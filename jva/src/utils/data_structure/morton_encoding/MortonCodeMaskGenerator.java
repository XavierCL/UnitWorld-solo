package utils.data_structure.morton_encoding;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.IntStream;

public class MortonCodeMaskGenerator {

    public static void main(String[] args) {
        final int dimensionality = 2;

        IntStream.range(0, dimensionality).forEach(dimensionNumber -> {
            System.out.println("private static final int[] mortonMaskDimension" + dimensionNumber + " = new int[] {");
            IntStream.range(0, 256).forEach(i -> {
                final AtomicInteger mortonValue = new AtomicInteger(0);
                IntStream.range(0, 8).forEach(bitNumber -> {
                    final int bitNumberAsByte = 1 << bitNumber;
                    final int currentBit = ((i & bitNumberAsByte) >> bitNumber);
                    final int bitShiftAmount = bitNumber * dimensionality + dimensionNumber;
                    mortonValue.set(mortonValue.get() + (currentBit << bitShiftAmount));
                });

                if((i) % 8 == 0) {
                    System.out.print("\t");
                }
                System.out.print(mortonValue.get());
                if(i != 255) {
                    System.out.print(", ");
                }
                if((i+1) % 8 == 0) {
                    System.out.println();
                }
            });
            System.out.println("};\n");
        });
    }
}
