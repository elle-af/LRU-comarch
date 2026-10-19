import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Scanner;

public class LRU {

    //LU
    // Frame table that evicts its least recently used page when it overflows
    static class LRUFrames extends LinkedHashMap<Integer, Boolean> {
        private final int capacity;
        Integer lastRemoved = null; // the page that was evicted (for display)

        LRUFrames(int capacity) {
            super(16, 0.75f, true); // true = access-order (not insertion-order)
            this.capacity = capacity;
        }

        @Override
        protected boolean removeEldestEntry(Map.Entry<Integer, Boolean> eldest) {
            if (size() > capacity) {
                lastRemoved = eldest.getKey(); // eldest = least recently used
                return true;                   // evict it
            }
            return false;
        }
    }

    //ALFARO
    // Greatest common divisor, used to reduce the hit ratio to lowest terms
    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
    // Prints the table (reference row, H/* row, then one row per used frame) up to column upTo
    static void printTable(int[] refs, boolean[] hit, int[][] table, int upTo, int numFrames) {
        StringBuilder sb = new StringBuilder();
        for (int j = 0; j <= upTo; j++) sb.append(String.format("%-5d", refs[j]));
        sb.append("\n");
        for (int j = 0; j <= upTo; j++) sb.append(String.format("%-5s", hit[j] ? "H" : "*"));
        sb.append("\n");
        int rows = 0; // only show frames that are in use at this step
        for (int f = 0; f < numFrames; f++) {
            if (table[f][upTo] != -1) rows++;
        }
        for (int f = 0; f < rows; f++) {
            StringBuilder row = new StringBuilder();
            for (int j = 0; j <= upTo; j++) {
                row.append(String.format("%-5s", table[f][j] == -1 ? "" : String.valueOf(table[f][j])));
            }
            sb.append(row.toString().stripTrailing()).append("\n");
        }
        System.out.print(sb);
    }


    //RECTO
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.println("=== LRU Page Replacement Simulator ===");

        // ---- Input 1: number of page frames (no upper limit) ----
        int numFrames = 0;
        while (numFrames <= 0) {
            System.out.print("Enter number of page frames: ");
            try {
                numFrames = Integer.parseInt(sc.nextLine().trim());
                if (numFrames <= 0) System.out.println("  Please enter a number greater than 0.");
            } catch (NumberFormatException e) {
                System.out.println("  Invalid number. Try again.");
            }
        }

        // ---- Input 2: reference string (each page is 1 to 3 digits) ----
        int[] refs = null;
        while (refs == null) {
            System.out.print("Enter reference string (1-3 digit pages, separated by spaces or commas): ");
            String[] tokens = sc.nextLine().trim().split("[,\\s]+");
            int[] temp = new int[tokens.length];
            boolean ok = tokens.length > 0 && !tokens[0].isEmpty();
            for (int i = 0; ok && i < tokens.length; i++) {
                if (!tokens[i].matches("\\d{1,3}")) {
                    System.out.println("  Invalid page \"" + tokens[i] + "\" (must be 1 to 3 digits). Try again.");
                    ok = false;
                } else {
                    temp[i] = Integer.parseInt(tokens[i]);
                }
            }
            if (ok) refs = temp;
        }

        //GLER
        int total = refs.length;
        // Header
        StringBuilder input = new StringBuilder();
        for (int i = 0; i < total; i++) {
            if (i > 0) input.append(", ");
            input.append(refs[i]);
        }
        System.out.println();
        System.out.println("Page frames: " + numFrames);
        System.out.println("Input: " + input);
        System.out.println();
        System.out.println("Press ENTER to show each step one at a time.");
        System.out.println("H = HIT, * = MISS (page loaded). Each row is one frame; blank = empty.");
        System.out.println("---------------------------------------------------------------");


        //MASILLAM
        LRUFrames cache = new LRUFrames(numFrames);
        int[] slots = new int[numFrames];            // what each frame currently holds
        for (int i = 0; i < numFrames; i++) slots[i] = -1;
        int[][] table = new int[numFrames][total];   // frame contents after every step
        boolean[] hit = new boolean[total];
        int hits = 0;


        //JALILULA
        for (int step = 0; step < total; step++) {
            sc.nextLine(); // wait for the user to press ENTER
            int page = refs[step];
            String result;

            // get() returns non-null if the page is loaded AND marks it most recently used
            if (cache.get(page) != null) {
                hit[step] = true;
                hits++;
                result = "HIT";
            } else {
                cache.lastRemoved = null;
                cache.put(page, true); // load page; may evict the LRU page
                int target = -1;
                if (cache.lastRemoved != null) {
                    // all frames full: the new page takes the removed page's frame
                    for (int i = 0; i < numFrames; i++) {
                        if (slots[i] == cache.lastRemoved) { target = i; break; }
                    }
                    result = "MISS, removed " + cache.lastRemoved;
                } else {
                    // a frame was still empty: use the first one
                    for (int i = 0; i < numFrames; i++) {
                        if (slots[i] == -1) { target = i; break; }
                    }
                    result = "MISS";
                }
                slots[target] = page;
            }


            //ARAWANGSA
            for (int f = 0; f < numFrames; f++) table[f][step] = slots[f];

            System.out.println("Step " + (step + 1) + "/" + total + ": page " + page + " -> " + result);
            printTable(refs, hit, table, step, numFrames);
            System.out.println("---------------------------------------------------------------");
        }

        // ---- Hit ratio, reduced to lowest terms ----
        int g = gcd(hits, total);
        System.out.println("Total references: " + total + " | Hits: " + hits + " | Misses: " + (total - hits));
        System.out.println("Hit ratio: " + hits + "/" + total + " = " + (hits / g) + "/" + (total / g)
                + " (" + String.format("%.2f", 100.0 * hits / total) + "%)");
    }
}
