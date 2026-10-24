import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.program.model.symbol.*;
import java.util.*;

public class NLMapper extends GhidraScript {
    public void run() {
        String target = "nl_gfx_graph_bool";
        System.out.println("Searching for string: " + target);
        
        // Search for the string in the program
        Listing listing = currentProgram.getListing();
        Address addr = findString(target);
        
        if (addr == null) {
            System.out.println("Could not find string " + target);
            return;
        }
        
        System.out.println("Found string at: " + addr);
        
        // Find references to this address
        Reference[] refs = getReferencesTo(addr);
        for (Reference ref : refs) {
            Address from = ref.getFromAddress();
            System.out.println("Reference found from: " + from);
            
            // Try to find the function containing this reference
            Function func = getFunctionContaining(from);
            if (func != null) {
                System.out.println("Contained in function: " + func.getName() + " at " + func.getEntryPoint());
                // In a real scenario, we would use the Decompiler here.
            }
        }
    }

    private Address findString(String s) {
        // Simple search for the bytes of the string
        byte[] pattern = s.getBytes();
        Memory mem = currentProgram.getMemory();
        for (Address a = mem.getMinAddress(); a != null && a.compareTo(mem.getMaxAddress()) < 0; a = a.add(1)) {
            boolean match = true;
            for (int i = 0; i < pattern.length; i++) {
                if (mem.getByte(a.add(i)) != pattern[i]) {
                    match = false;
                    break;
                }
            }
            if (match) return a;
        }
        return null;
    }
}
