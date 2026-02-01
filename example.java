class example {
	public static int test() {
		return 20;
	}
	public static int test2() {
		return test() + 20;
	}
	public static int main(String[] args) {
		int a = 100 + 200;
		a += test2();
		return a;
	}
}