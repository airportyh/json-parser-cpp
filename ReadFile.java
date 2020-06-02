import java.io.*;

public class ReadFile {
    public static void main(String[] args) throws Exception {
        File file = new File("unicode.txt");
        BufferedReader br = new BufferedReader(new FileReader(file));
        String line = br.readLine();
        for (int i = 0; i < line.length(); i++) {
            char chr = line.charAt(i);
            int code = chr;
            String binary = padLeftZeros(Integer.toBinaryString(code), 16);
            System.out.println(chr + "\t" + code + "\t" + binary);
        }
        System.out.println(line);
    }

    public static String padLeftZeros(String inputString, int length) {
        if (inputString.length() >= length) {
            return inputString;
        }
        StringBuilder sb = new StringBuilder();
        while (sb.length() < length - inputString.length()) {
            sb.append('0');
        }
        sb.append(inputString);
    
        return sb.toString();
    }
}