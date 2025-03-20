struct nested_struct {
  int a;
  struct {
    int b;
  } inner;
};

int f(struct nested_struct s) {
  return s.a + s.inner.b;
}
