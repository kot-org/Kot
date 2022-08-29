public class Test {

    public static int[] arr = new int[100000];

    public static void main(String[] args) {
        System.out.println("hello world!");
        arr[0] = 20 * 20;
        arr[1] = 10 * 2;
        arr[99998] = 99;
        System.out.println("" + arr[0]);
        System.out.println(arr[1]);
        System.out.println(arr[99998]);
    }
    
}
