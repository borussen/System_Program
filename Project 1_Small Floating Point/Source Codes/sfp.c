#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "sfp.h"

struct sfpbits {
	union {
		struct {
			sfp F : 10;  
			sfp E : 5;  
			sfp S : 1;  
		};  
		sfp SEF;
	};
};

sfp return_E(int input) {
	sfp E = 0;

	while (input > 1) {
		input = input / 2;
		E++;
	}

	return E;
}

sfp return_exp(int input) {
	sfp E;
	E = return_E(input);

	return 15 + E;  
}



sfp int2sfp(int input) {

	struct sfpbits k = { 0, };

	if (input > 65504) {
		k.F = 0;   
		k.E = 31;  
		k.S = 0;   
	}

	else if (input < -65504) {
		k.F = 0;   
		k.E = 31;  
		k.S = 1;   
	}

	else if (input == 0) {
		k.F = 0;
		k.E = 0;
		k.S = 0;
	}

	else {
		if (input > 0) {
			k.F = input;
			for (int i = 0; i < 10 - return_E(input); i++) {  
				k.F *= 2;
			}
			k.E = return_exp(input);
			k.S = 0;   
		}
		else {
			k.F = -input;
			for (int i = 0; i < 10 - return_E(-input); i++) {  
				k.F *= 2;
			}
			k.E = return_exp(-input);
			k.S = 1;   
		}
	}

	return k.SEF;
}

int sfp2int(sfp input) {

	int output;
	struct sfpbits k = { 0, };

	k.SEF = input;

	if (k.E == 31) {
		if (k.F == 0) {
			if (k.S == 0) {  
				output = INT_MAX; 
			}
			else {  
				output = INT_MIN; 
			}
		}
		else {
			output = INT_MIN; 
		}
	}

	else if (k.E == 0) {
		output = 0;
	}

	else {
		float frac = (float)(1024 + k.F) / 1024;  
		int exp = k.E - 15; 

		if (exp > 0) {
			for (int i = 0; i < exp; i++) {
				frac *= 2;
			}
		}
		else if (exp < 0) {
			for (int i = 0; i < -exp; i++) {
				frac /= 2;
			}
		}

		if (k.S == 0) {
			output = (int)(frac);
		}
		else {
			output = (int)(-frac);
		}
	}

	return output;
}

sfp float2sfp(float input) {

	struct sfpbits k = { 0, };

	if (input > 65504) {
		k.F = 0;
		k.E = 31;
		k.S = 0;
	}

	else if (input < -65504) {
		k.F = 0;   
		k.E = 31;  
		k.S = 1;   
	}

	else if ((-0.000061 < input) && (input < 0)) {
		k.E = 0;
		k.S = 1;
		float Real = -input;

		Real *= -16384;  
		
		for (int i = 0; i < 10; i++) {  
			if (Real * 2 >= 1) {
				int k_Real = 1;
				for (int j = 0; j < 9 - i; j++) {
					k_Real *= 2;
				}
				k.F += k_Real;
				Real = 2 * Real - 1;
			}
			else {
				Real *= 2;
			}
		}  
	}

	else if ((0 <= input) && (input < 0.000061)) {
		k.E = 0;
		k.S = 0;
		float Real = input;

		Real *= 16384;  

		for (int i = 0; i < 10; i++) {  
			if (Real * 2 >= 1) {
				int k_Real = 1;
				for (int j = 0; j < 9 - i; j++) {
					k_Real *= 2;
				}
				k.F += k_Real;
				Real = 2 * Real - 1;
			}
			else {
				Real *= 2;
			}
		}
	}

	else {
		if (input >= 0) {

			int num = (int)(input);  
			float Real = input - num;  

			k.S = 0;
			k.F = num;

			for (int i = 0; i < 10 - return_E(num); i++) {
				k.F *= 2;
			}  

			for (int i = 0; i < 10 - return_E(num); i++) {
				if (Real * 2 >= 1) {
					int k_Real = 1;
					for (int j = 0; j < 9 - return_E(num) - i; j++) {
						k_Real *= 2;
					}
					k.F += k_Real;
					Real = 2 * Real - 1;
				}
				else {
					Real *= 2;
				}
			}  

			if (input < 1) {
				int E = 0;
				while (input < 1) {
					input *= 2;
					E--;
				}
				k.E = 15 + E;

				for (int o = 0; o < -E; o++) {
					k.F *= 2;
				}
			}

			else {
				k.E = return_exp(num);
			}
		}

		else {
			int num = (int)(input);
			float Real = -(input - num);

			k.S = 1;
			k.F = -num;

			for (int i = 0; i < 10 - return_E(-num); i++) {
				k.F *= 2;
			} 

			for (int i = 0; i < 10 - return_E(-num); i++) {
				if (Real * 2 >= 1) {
					int k_Real = 1;
					for (int j = 0; j < 9 - return_E(-num) - i; j++) {
						k_Real *= 2;
					}
					k.F = k.F + k_Real;
					Real = 2 * Real - 1;
				}
				else {
					Real *= 2;
				}
			}  

			if (-input < 1) {
				int E = 0;
				while (-input < 1) {
					input *= 2;
					E--;
				}
				k.E = 15 + E;

				for (int o = 0; o < -E; o++) {
					k.F *= 2;
				}
			}
			else {
				k.E = return_exp(-num);
			}
		}
	}

	return k.SEF;
}

float sfp2float(sfp input) {

	float output;
	struct sfpbits k = { 0, };

	k.SEF = input;
	int exp = k.E - 15;

	if (k.E == 0) {
		float frac = (float)(k.F) / 1024;

		for (int i = 0; i < 14; i++) {   
			frac = frac / 2;
		}

		if (k.S == 0) {
			output = frac;
		}
		else {
			output = -frac;
		}
	}


	else {
		float frac = (float)(k.F + 1024) / 1024;

		if (exp >= 0) {
			for (int i = 0; i < exp; i++) {
				frac = frac * 2;
			}

			if (k.S == 0) {
				output = frac;
			}
			else {
				output = -frac;
			}
		}

		else {
			for (int i = 0; i < -exp; i++) {
				frac = frac / 2;
			}

			if (k.S == 0) {
				output = frac;
			}
			else {
				output = -frac;
			}
		}

	}

	return output;
}

sfp sfp_add(sfp a, sfp b) {
	sfp output;
	int shift;

	struct sfpbits A = { 0, };
	struct sfpbits B = { 0, };
	struct sfpbits O = { 0, };

	A.SEF = a;
	B.SEF = b;

	if ((A.E == 31 && A.F != 0) || (B.E == 31 && B.F != 0)) {
		output = 31745;
	}

	else if (((A.S == 0 && A.E == 31 && A.F == 0) && (B.S == 1 && B.E == 31 && B.F == 0)) || ((A.S == 1 && A.E == 31 && A.F == 0) && (B.S == 0 && B.E == 31 && B.F == 0))) {
		output = 31745;
	}

	else if ((A.S == 0 && A.E == 31 && A.F == 0) || (B.S == 0 && B.E == 31 && B.F == 0)) {
		output = 31744;
	}

	else if ((A.S == 1 && A.E == 31 && A.F == 0) || (B.S == 1 && B.E == 31 && B.F == 0)) {
		output = 64512;
	}

	else if ((A.E == 0) || (B.E == 0)) {
		sfp Af = A.F;
		sfp Bf = B.F;
		sfp Ae = A.E;
		sfp Be = B.E;

		if (A.E == B.E) {
			if (A.S == 0 && B.S == 0) {
				sfp Of = Af + Bf;
				O.F = Of;
				O.S = 0;

				if (Of > 1023) {
					O.E = 1;
				}
				else {
					O.E = 0;
				}
			}

			else if (A.S == 0 && B.S == 1) {
				if (a - b + 32768 >= 0) {
					O.F = Af - Bf;
					O.E = 0;
					O.S = 0;
				}
				else {
					O.F = Bf - Af;
					O.E = 0;
					O.S = 1;
				}
			}

			else if (A.S == 1 && B.S == 0) {
				if (b - a + 32768 >= 0) {
					O.F = Bf - Af;
					O.E = 0;
					O.S = 0;
				}
				else {
					O.F = Af - Bf;
					O.E = 0;
					O.S = 1;
				}
			}

			else {
				sfp Of = Af + Bf;
				O.F = Of;
				O.S = 1;

				if (Of > 1023) {
					O.E = 1;
				}
				else {
					O.E = 0;
				}
			}
		}

		else {  

			if (A.S == 0 && B.S == 0) {  
				O.S = 0;

				if (A.E >= B.E) {
					shift = A.E - B.E - 1;  

					for (int i = 0; i < shift; i++) { 
						Bf /= 2;
					}

					sfp Of = Af + 1024 + Bf;  

					while (Of > 2047) {  
						Of /= 2;
						Ae += 1;
					}

					if (Ae >= 31) {  
						O.E = 31;
						O.F = 0;
					}
					else {
						O.E = Ae;
						O.F = Of;
					}
				}

				else {
					shift = B.E - A.E - 1;

					for (int i = 0; i < shift; i++) {
						Af /= 2;
					}

					sfp Of = Bf + 1024 + Af;

					while (Of > 2047) {
						Of /= 2;
						Be += 1;
					}

					if (Be >= 31) {
						O.E = 31;
						O.F = 0;
					}
					else {
						O.E = Be;
						O.F = Of;
					}
				}
			}

			else if (A.S == 0 && B.S == 1) {  
				if (a - b + 32768 >= 0) {  
					O.S = 0;

					shift = A.E - B.E - 1;

					for (int i = 0; i < shift; i++) {
						Bf /= 2;
					}

					sfp Of = Af + 1024 - Bf;

					if (A.E == 1) { 
						if (Of < 1024) {
							O.E = 0;
						}
						else {
							O.E = A.E;
						}
						O.F = Of;
					}

					else {
						while (Of < 1024) {  
							Of *= 2;
							A.E -= 1;
						}
						O.E = A.E;
						O.F = Of;
					}
				}

				else {
					O.S = 1;

					shift = B.E - A.E - 1;

					for (int i = 0; i < shift; i++) {
						Af /= 2;
					}

					sfp Of = Bf + 1024 - Af;

					if (B.E == 1) {
						if (Of < 1024) {
							O.E = 0;
						}
						else {
							O.E = B.E;
						}
						O.F = Of;
					}

					else {
						while (Of < 1024) {
							Of *= 2;
							B.E -= 1;
						}
						O.E = B.E;
						O.F = Of;
					}
				}
			}

			else if (A.S == 1 && B.S == 0) {
				if (b - a + 32768 >= 0) {
					O.S = 0;

					shift = B.E - A.E - 1;

					for (int i = 0; i < shift; i++) {
						Af /= 2;
					}

					sfp Of = Bf + 1024 - Af;

					if (B.E == 1) {
						if (Of < 1024) {
							O.E = 0;
						}
						else {
							O.E = B.E;
						}
						O.F = Of;
					}
					else {
						while (Of < 1024) {
							Of *= 2;
							B.E -= 1;
						}
						O.E = B.E;
						O.F = Of;
					}
				}

				else {
					O.S = 1;

					shift = A.E - B.E - 1;

					for (int i = 0; i < shift; i++) {
						Bf /= 2;
					}

					sfp Of = Af + 1024 - Bf;

					if (A.E == 1) {
						if (Of < 1024) {
							O.E = 0;
						}
						else {
							O.E = A.E;
						}
						O.F = Of;
					}
					else {
						while (Of < 1024) {
							Of *= 2;
							A.E -= 1;
						}
						O.E = A.E;
						O.F = Of;
					}
				}

			}

			else {
				O.S = 1;
				if (A.E >= B.E) {
					shift = A.E - B.E - 1;

					for (int i = 0; i < shift; i++) {
						Bf /= 2;
					}

					sfp Of = Af + 1024 + Bf;

					while (Of > 2047) {
						Of /= 2;
						Ae += 1;
					}

					if (Ae >= 31) {
						O.E = 31;
						O.F = 0;
					}
					else {
						O.E = Ae;
						O.F = Of;
					}
				}

				else {
					shift = B.E - A.E - 1;

					for (int i = 0; i < shift; i++) {
						Af /= 2;
					}

					sfp Of = Bf + 1024 + Af;

					while (Of > 2047) {
						Of /= 2;
						Be += 1;
					}

					if (Be >= 31) {
						O.E = 31;
						O.F = 0;
					}
					else {
						O.E = Be;
						O.F = Of;
					}
				}
			}
		}
		output = O.SEF;
	}

	else {
		sfp Af = A.F;
		sfp Bf = B.F;
		sfp Ae = A.E;
		sfp Be = B.E;

		if (A.S == 0 && B.S == 0) {  
			O.S = 0;

			if (A.E >= B.E) {
				shift = A.E - B.E;
				Bf += 1024;  

				for (int i = 0; i < shift; i++) { 
					Bf /= 2;
				}

				sfp Of = Af + 1024 + Bf;  

				while (Of > 2047) {
					Of /= 2;
					Ae += 1;
				}

				if (Ae >= 31) {  
					O.E = 31;
					O.F = 0;
				}
				else {
					O.E = Ae;
					O.F = Of;
				}
			}

			else {
				shift = B.E - A.E;
				Af += 1024;

				for (int i = 0; i < shift; i++) {
					Af /= 2;
				}

				sfp Of = Bf + 1024 + Af;

				while (Of > 2047) {
					Of /= 2;
					Be += 1;
				}

				if (Be >= 31) {
					O.E = 31;
					O.F = 0;
				}
				else {
					O.E = Be;
					O.F = Of;
				}
			}
		}

		else if (A.S == 0 && B.S == 1) {  
			if (a - b + 32768 >= 0) {  
				O.S = 0;

				shift = A.E - B.E;
				Bf += 1024;

				for (int i = 0; i < shift; i++) {  
					Bf /= 2;
				}

				sfp Of = Af + 1024 - Bf;

				while (Of < 1024) {  
					if (A.E == 1) {  
						if ((0 <= Of) && (Of < 1024)) {  
							A.E = 0;
							break;
						}
					}
					else {
						Of *= 2;
						A.E -= 1;
					}
				}
				O.E = A.E;
				O.F = Of;
			}

			else {
				O.S = 1;

				shift = B.E - A.E;
				Af += 1024;

				for (int i = 0; i < shift; i++) {
					Af /= 2;
				}

				sfp Of = Bf + 1024 - Af;

				while (Of < 1024) {
					if (B.E == 1) {
						if ((0 <= Of) && (Of < 1024)) {
							B.E = 0;
							break;
						}
					}
					else {
						Of *= 2;
						B.E -= 1;
					}
				}
				O.E = B.E;
				O.F = Of;
			}
		}

		else if (A.S == 1 && B.S == 0) {
			if (b - a + 32768 >= 0) {
				O.S = 0;

				shift = B.E - A.E;
				Af += 1024;

				for (int i = 0; i < shift; i++) {
					Af /= 2;
				}

				sfp Of = Bf + 1024 - Af;

				while (Of < 1024) {
					if (B.E == 1) {
						if ((0 <= Of) && (Of < 1024)) {
							B.E = 0;
							break;
						}
					}
					else {
						Of *= 2;
						B.E -= 1;
					}
				}
				O.E = B.E;
				O.F = Of;
			}

			else {
				O.S = 1;

				shift = A.E - B.E;
				Bf += 1024;

				for (int i = 0; i < shift; i++) {
					Bf /= 2;
				}

				sfp Of = Af + 1024 - Bf;

				while (Of < 1024) {
					if (A.E == 1) {
						if ((0 <= Of) && (Of < 1024)) {
							A.E = 0;
							break;
						}
					}
					else {
						Of *= 2;
						A.E -= 1;
					}
				}
				O.E = A.E;
				O.F = Of;
			}

		}

		else {
			O.S = 1;
			if (A.E >= B.E) {
				shift = A.E - B.E;
				Bf += 1024;

				for (int i = 0; i < shift; i++) {
					Bf /= 2;
				}

				sfp Of = Af + 1024 + Bf;

				while (Of > 2047) {
					Of /= 2;
					Ae += 1;
				}

				if (Ae >= 31) {
					O.E = 31;
					O.F = 0;
				}
				else {
					O.E = Ae;
					O.F = Of;
				}
			}

			else {
				shift = B.E - A.E;
				Af += 1024;

				for (int i = 0; i < shift; i++) {
					Af /= 2;
				}

				sfp Of = Bf + 1024 + Af;

				while (Of > 2047) {
					Of /= 2;
					Be += 1;
				}

				if (Be >= 31) {
					O.E = 31;
					O.F = 0;
				}
				else {
					O.E = Be;
					O.F = Of;
				}
			}
		}

		output = O.SEF;
	}

	return output;
}

sfp sfp_mul(sfp a, sfp b) {
	sfp output;

	struct sfpbits A = { 0, };
	struct sfpbits B = { 0, };
	struct sfpbits O = { 0, };

	A.SEF = a;
	B.SEF = b;

	sfp aE = A.E;
	sfp bE = B.E;

	if ((A.E == 31 && A.F != 0) || (B.E == 31 && B.F != 0)) {
		output = 31745;
	}

	else if (((A.E == 31 && A.F == 0) && (B.E == 0 && B.F == 0)) || ((A.E == 0 && A.F == 0) && (B.E == 31 && B.F == 0))) {
		output = 31745;
	}

	else if (((A.S == 0 && A.E == 31 && A.F == 0) && B.S == 0) || ((A.S == 1 && A.E == 31 && A.F == 0) && B.S == 1) || (A.S == 0 && (B.S == 0 && B.E == 31 && B.F == 0)) || (A.S == 1 && (B.S == 1 && B.E == 31 && B.F == 0))) {
		output = 31744;
	}

	else if (((A.S == 0 && A.E == 31 && A.F == 0) && B.S == 1) || ((A.S == 1 && A.E == 31 && A.F == 0) && B.S == 0) || (A.S == 0 && (B.S == 1 && B.E == 31 && B.F == 1)) || (A.S == 1 && (B.S == 0 && B.E == 31 && B.F == 0))) {
		output = 64512;
	}
	else if (A.E == 0 || B.E == 0) {  
		if (A.E == B.E) {  
			O.E = 0;
			O.F = 0;

			if (A.S == B.S) {
				O.S = 0;
			}
			else {
				O.S = 1;
			}
		}

		else if (A.E == 0) {  
			int Af = A.F;
			int oE = bE - 15;
			int oF = A.F * (B.F + 1024);  

			if (A.F == 0) {  
				O.E = 0;
				O.F = 0;

				if (A.S == B.S) {
					O.S = 0;
				}
				else {
					O.S = 1;
				}
			}

			else {
				oF /= 1024;

				for (int i = 0; i < oE; i++) {
					oF *= 2;
				}

				if (oF < 1024) { 
					O.E = 0;
					O.F = oF;

					if (A.S == B.S) {
						O.S = 0;
					}
					else {
						O.S = 1;
					}
				}

				else {  
					int j = 0;
					while (oF > 1023) {
						oF /= 2;
						j++;
					}

					O.E = j + 1;
					O.F = oF;

					if (A.S == B.S) {
						O.S = 0;
					}
					else {
						O.S = 1;
					}
				}

			}
			output = O.SEF;
		}

		else {
			int Bf = B.F;
			int oE = aE - 15;
			int oF = B.F * (A.F + 1024);

			if (B.F == 0) {
				O.E = 0;
				O.F = 0;

				if (A.S == B.S) {
					O.S = 0;
				}
				else {
					O.S = 1;
				}
			}

			else {
				oF /= 1024;

				for (int i = 0; i < oE; i++) {
					oF *= 2;
				}

				if (oF < 1024) {
					O.E = 0;
					O.F = oF;

					if (A.S == B.S) {
						O.S = 0;
					}
					else {
						O.S = 1;
					}
				}

				else {
					int j = 0;
					while (oF > 1023) {
						oF /= 2;
						j++;
					}

					O.E = j + 1;
					O.F = oF;

					if (A.S == B.S) {
						O.S = 0;
					}
					else {
						O.S = 1;
					}
				}

			}
			output = O.SEF;
		}
	}

	else {    
		int oE = aE + bE - 15;   
		int oF = (A.F + 1024) * (B.F + 1024);   

		while (oF > 2096128) {  
			oF /= 2;
			oE++;
		}

		while (oF > 2047) {
			oF /= 2;
		}

		if (oE < 0) {  
			while (oE < 0) {
				oF /= 2;
				oE++;
			}
			O.E = oE;
			O.F = oF / 2;

			if (A.S == B.S) {
				O.S = 0;
			}
			else {
				O.S = 1;
			}
		}

		else if (oE >= 31) {  
			O.E = 31;
			O.F = 0;

			if (A.S == B.S) {
				O.S = 0;
			}
			else {
				O.S = 1;
			}
		}

		else {
			O.E = oE;
			O.F = oF;
			
			if (A.S == B.S) {
				O.S = 0;
			}
			else {
				O.S = 1;
			}
		}
		output = O.SEF;
	}
	return output;
}

char* sfp2bits(sfp result) {

	int i, index = 1;
	char* bit = (char*)malloc(sizeof(char) * 17);

	memset(bit, '0', 17 * sizeof(char));
	bit[16] = '\0';        // the end of string

	for (i = 15; i >= 0; i--) {
		if (result % 2 == 0) {
			bit[i] = '0';
		}
		else {
			bit[i] = '1';
		}
		result /= 2;
	}

	return bit;
}
