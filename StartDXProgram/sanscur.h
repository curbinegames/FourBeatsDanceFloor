#pragma once

typedef struct cint_s {
	int real; /* real number */
	int e; /* r * 10^e */
} cint_t;

cint_t cint_add(cint_t a, cint_t b);
int flip_cint(cint_t num);

static cint_t cint_add(cint_t a, cint_t b) {
	cint_t ret;
	while (a.e != b.e) {
		if (a.e < b.e) {
			a.real /= 10;
			a.e++;
		}
		else {
			b.real /= 10;
			b.e++;
		}
	}
	while (1) {
		ret.real = a.real + b.real;
		if (ret.real - b.real == a.real) {
			break;
		}
		a.real /= 10;
		a.e++;
		b.real /= 10;
		b.e++;
	}
	ret.e = a.e;
	return ret;
}

static cint_t cint_sub(cint_t a, cint_t b) {
	cint_t ret;
	while (a.e != b.e) {
		if (a.e < b.e) {
			a.real /= 10;
			a.e++;
		}
		else {
			b.real /= 10;
			b.e++;
		}
	}
	ret.real = a.real - b.real;
	ret.e = a.e;
	return ret;
}

static cint_t cint_mlp(cint_t a, cint_t b) {
	cint_t ret;
	while (1) {
		ret.real = a.real * b.real;
		if (ret.real == 0) {
			ret.e = 0;
			return ret;
		}
		if (ret.real / b.real == a.real) {
			break;
		}
		if (a.real > b.real) {
			a.real /= 10;
			a.e++;
		}
		else {
			b.real /= 10;
			b.e++;
		}
	}
	ret.e = a.e + b.e;
	return ret;
}

static cint_t cint_div(cint_t a, cint_t b) {
	cint_t ret;
	ret.real = a.real / b.real;
	ret.e = a.e - b.e;
	return ret;
}

static int flip_cint(cint_t num) {
	int ret = num.real;
	while (num.e > 0) {
		ret *= 10;
		num.e--;
	}
	return ret;
};
