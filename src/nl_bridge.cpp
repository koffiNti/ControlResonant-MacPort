#include "nl_api_blueprint.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

// V8 includes
#include <v8.h>
#include <libplatform/libplatform.h>

// Metal/Apple SDK includes
#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>
#import <CoreVideo/CoreVideo.h>

// Forward declaration for the parser
namespace NLBridge {
    class BinaryParser {
    public:
        struct ExtractionResult {
            bool success;
            std::vector<uint8_t> payload;
            std::string metadata;
        };
        static ExtractionResult Extract(const std::string& filePath);
    };
}

namespace NLBridge {

    // --- Internal State ---
    struct InternalState {
        std::unordered_map<EntityID, std::string> entities;
        std::unordered_map<AssetHandle, std::vector<uint8_t>> loaded_assets;
        uint64_t next_asset_id = 1;

        // V8 Engine
        v8::Isolate* isolate = nullptr;
        v8::Isolate::CreateParams create_params;
        v8::Platform* platform = nullptr;
        v8::Global<v8::Context> context = v8::Global<v8::Context>();

        // Metal
        id<MTLDevice> device = nullptr;
        id<MTLCommandQueue> commandQueue = nullptr;
        id<MTLBuffer> globalSharedBuffer = nullptr;
        size_t currentBufferSize = 0;

        // Windowing & Display
        NSWindow* window = nullptr;
        CAMetalLayer* metalLayer = nullptr;
        CVDisplayLinkRef displayLink = nullptr;

        // State
        RenderState render_state;
    };

    static InternalState state;

    // --- Native Binding Glue ---
    void Native_nl_info(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.length() > 0) {
            v8::String::Utf8Value str(isolate, args[0]);
            NLBridge::nl_info(*str);
        }
    }

    void Native_nl_window_set_title(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.length() > 0) {
            v8::String::Utf8Value str(isolate, args[0]);
            NLBridge::nl_window_set_title(*str);
        }
    }

    void RegisterNativeBindings() {
        v8::Isolate* isolate = state.isolate;
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = state.context.Get(isolate);
        v8::Context::Scope context_scope(context);
        v8::Local<v8::Object> global = context->Global();

        global->Set(context, v8::String::NewFromUtf8(isolate, "nl_info").ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, Native_nl_info)->GetFunction(context).ToLocalChecked()).Check();

        global->Set(context, v8::String::NewFromUtf8(isolate, "nl_window_set_title").ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, Native_nl_window_set_title)->GetFunction(context).ToLocalChecked()).Check();

        nl_info("Native JS bindings registered (including Windowing).");
    }

    // --- Windowing implementation ---
    void nl_window_set_title(const std::string& title) {
        if (state.window) {
            [state.window setTitle:[NSString stringWithUTF8String:title.c_str()]];
        }
    }

    void nl_window_set_size(int width, int height) {
        if (state.window) {
            [state.window setContentSize:NSMakeSize(width, height)];
        }
    }

    void nl_window_set_fullscreen(bool enabled) {
        if (state.window) {
            if (enabled) [state.window toggleFullScreen:nil];
        }
    }

    // --- DisplayLink Callback ---
    static void DisplayLinkCallback(CVDisplayLinkRef dl, CVDisplayLinkGetCurrentTime l_time,
                                   uint64_t rightFrame, uint64_t leftFrame, void* userData) {
        // This is the "Heartbeat" of the engine.
        // It triggers exactly when the monitor is ready for a new frame.
        // The RenderLoop will now be driven by this callback.
    }

    // Initialize V8
    bool InitializeV8() {
        state.platform = v8::platform::GetDefaultPlatform();
        v8::V8::InitializePlatform(state.platform);
        v8::V8::Initialize();
        state.create_params.constraints.set_max_old_space_size(1024 * 1024 * 512);
        state.isolate = v8::Isolate::New(state.create_params);
        v8::Isolate::Scope isolate_scope(state.isolate);
        v8::HandleScope handle_scope(state.isolate);
        state.context.Set(v8::Context::New(state.isolate));
        RegisterNativeBindings();
        nl_info("V8 Engine initialized.");
        return true;
    }

    // Initialize Metal
    bool InitializeMetal() {
        state.device = MTLCreateSystemDefaultDevice();
        if (!state.device) return false;
        state.commandQueue = [state.device newCommandQueue];
        state.currentBufferSize = 128 * 1024 * 1024;
        state.globalSharedBuffer = [state.device newBufferWithLength:state.currentBufferSize options:MTLResourceStorageModeShared];
        nl_info("Metal UMA Foundation initialized.");
        return true;
    }

    // Initialize Window and DisplayLink
    bool InitializeHostWindow() {
        // 1. Create Window
        NSRect frame = NSMakeRect(0, 0, 1280, 720);
        state.window = [[NSWindow alloc] initWithContentRect:frame
                                                  styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
        [state.window setTitle:@"Control Resonant Mac Port"];
        [state.window makeKeyAndOrderFront:nil];

        // 2. Create Metal Layer
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = state.device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.framebufferOnly = YES;

        // Attach layer to window
        // (Note: In a full AppKit app, this would be in a custom NSView)

        // 3. Initialize CVDisplayLink
        CVDisplayLinkCreateWithMainDisplay(&state.displayLink);
        CVDisplayLinkSetCurrentDisplay(state.displayLink, kCVTDefaultDisplay);
        CVDisplayLinkSetOutputCallback(state.displayLink, DisplayLinkCallback, &state);
        CVDisplayLinkStart(state.displayLink);

        nl_info("macOS Host Window and DisplayLink initialized.");
        return true;
    }

    void ShutdownV8() {
        state.context.Reset();
        if (state.isolate) state.isolate->Dispose();
        v8::V8::Dispose();
        v8::V8::ShutdownPlatform();
    }

    void ShutdownMetal() {
        state.globalSharedBuffer = nil;
        state.commandQueue = nil;
        state.device = nil;
        nl_info("Metal resources released.");
    }

    void ShutdownHost() {
        if (state.displayLink) CVDisplayLinkStop(state.displayLink);
        if (state.window) [state.window close];
        nl_info("Host Window and DisplayLink shut down.");
    }

    double nl_time() { return 0.0; }
    float nl_delta_time() { return 0.016f; }
    void nl_info(const std::string& message) {
        std::cout << "[NLBridge INFO] " << message << std::endl;
    }

    // --- Asset Management ---
    AssetHandle nl_asset_load(const std::string& path, AssetType type) {
        nl_info("Loading asset: " + path);
        if (type == AssetType::BinaryContainer) {
            auto result = BinaryParser::Extract(path);
            if (result.success) {
                AssetHandle handle = state.next_asset_id++;
                state.loaded_assets[handle] = result.payload;
                return handle;
            }
            return 0;
        }
        AssetHandle handle = state.next_asset_id++;
        state.loaded_assets[handle] = {};
        return handle;
    }

    void nl_asset_unload(AssetHandle handle) { state.loaded_assets.erase(handle); }
    bool nl_asset_exists(const std::string& path) { return true; }
    void nl_entity_bind_asset(EntityID entity_id, AssetHandle handle) {
        state.entities[entity_id] = "Asset_" + std::to_string(handle);
    }

    bool nl_v8_load_script(AssetHandle script_handle, const std::string& module_name) {
        if (!state.isolate) return false;
        if (state.loaded_assets.find(script_handle) == state.loaded_assets.end()) return false;
        const std::vector<uint8_t>& binary_data = state.loaded_assets[script_handle];
        std::string js_source(binary_data.begin(), binary_data.end());
        v8::Isolate::Scope isolate_scope(state.isolate);
        v8::HandleScope handle_scope(state.isolate);
        v8::Local<v8::Context> context = state.context.Get(isolate_scope.isolate());
        v8::Context::Scope context_scope(context);
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_scope.isolate(), js_source.c_str()).ToLocalChecked();
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        script->Run(context).ToLocalChecked();
        return true;
    }

    bool nl_v8_execute_function(const std::string& func_name, const std::vector<std::string>& args) {
        v8::Isolate::Scope isolate_scope(state.isolate);
        v8::HandleScope handle_scope(state.isolate);
        v8::Local<v8::Context> context = state.context.Get(state.isolate);
        v8::Context::Scope context_scope(context);
        v8::Local<v8::Value> global = context->Global();
        v8::Local<v8::Value> func = global->Get(context, v8::String::NewFromUtf8(state.isolate, func_name.c_str()).ToLocalChecked()).ToLocalChecked();
        if (func->IsFunction()) {
            v8::Local<v8::Function> js_func = v8::Local<v8::Function>::Cast(func);
            return true;
        }
        return false;
    }

    void nl_mesh_show(EntityID id) { nl_info("Mesh show: " + std::to_string(id)); }
    void nl_mesh_hide(EntityID id) { nl_info("Mesh hide:, " + std::to_string(id)); }
}
