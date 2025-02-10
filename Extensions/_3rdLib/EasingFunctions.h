#pragma once

// Origin version by Anders Riggelsen (Andos)

namespace Easing {
	//Easing parameter value struct
	struct EaseVars {
		double overshoot;
		double amplitude;
		double period;
	};

	inline double linear(double step, EaseVars vars) { return step; }
	inline double quad(double step, EaseVars vars) { return pow(step, 2.0); }
	inline double cubic(double step, EaseVars vars) { return pow(step, 3.0); }
	inline double quart(double step, EaseVars vars) { return pow(step, 4.0); }
	inline double quint(double step, EaseVars vars) { return pow(step, 5.0); }
	inline double sine(double step, EaseVars vars) { return 1.0 - sin((1 - step) * 90.0 * PI / 180.0); }
	inline double expo(double step, EaseVars vars) { return pow(2.0, step * 10.0) / 1024.0; }
	inline double circ(double step, EaseVars vars) { return 1.0f - sqrt(1.0 - pow(step, 2.0)); }
	inline double back(double step, EaseVars vars) { return (vars.overshoot + 1.0) * pow(step, 3.0) - vars.overshoot * pow(step, 2.0); }
	inline double elastic(double step, EaseVars vars) {
		step -= 1.0;
		float amp = (float)(std::max)(1.0, vars.amplitude);
		float s = (float)(vars.period / (2.0 * PI) * asin(1.0 / amp));
		return -(amp * pow(2.0, 10 * step) * sin((step - s) * (2 * PI) / vars.period));
	}
	inline double bounce(double step, EaseVars vars) {
		step = 1 - step;
		if (step < (8 / 22.0))
			return 1 - 7.5625 * step * step;
		else if (step < (16 / 22.0)) {
			step -= 12 / 22.0;
			return 1 - vars.amplitude * (7.5625 * step * step + 0.75) - (1 - vars.amplitude);
		}
		else if (step < (20 / 22.0)) {
			step -= 18 / 22.0;
			return 1 - vars.amplitude * (7.5625 * step * step + 0.9375) - (1 - vars.amplitude);
		}
		else {
			step -= 21 / 22.0;
			return 1 - vars.amplitude * (7.5625 * step * step + 0.984375) - (1 - vars.amplitude);
		}
	}

	inline double doFunction(int number, double step, EaseVars vars) {
		switch (number) {
		default:
		case 0: return linear(step, vars);
		case 1: return quad(step, vars);
		case 2: return cubic(step, vars);
		case 3: return quart(step, vars);
		case 4: return quint(step, vars);
		case 5: return sine(step, vars);
		case 6: return expo(step, vars);
		case 7: return circ(step, vars);
		case 8: return back(step, vars);
		case 9: return elastic(step, vars);
		case 10: return bounce(step, vars);
		}
	}

	inline int StrToFunctionID(const wchar_t* pStr) {
		do {
			if (StrIEqu(pStr, L"linear")) {
				return  0;
			}

			if (StrIEqu(pStr, L"quad")) {
				return  1;
			}

			if (StrIEqu(pStr, L"cubic")) {
				return  2;
			}

			if (StrIEqu(pStr, L"quart")) {
				return  3;
			}

			if (StrIEqu(pStr, L"quint")) {
				return  4;
			}

			if (StrIEqu(pStr, L"sine")) {
				return  5;
			}

			if (StrIEqu(pStr, L"expo")) {
				return  6;
			}

			if (StrIEqu(pStr, L"circ")) {
				return  7;
			}

			if (StrIEqu(pStr, L"back")) {
				return  8;
			}

			if (StrIEqu(pStr, L"elastic")) {
				return  9;
			}

			if (StrIEqu(pStr, L"bounce")) {
				return  10;
			}

			return 0;
		} while (false);
	}

	inline double easeIn(int function, double step, EaseVars vars) {
		return doFunction(function, step, vars);
	}

	inline double easeOut(int function, double step, EaseVars vars) {
		return 1.0 - doFunction(function, 1.0 - step, vars);
	}

	inline double easeInOut(int functionA, int functionB, double step, EaseVars vars) {
		if (step < 0.5)
			return easeIn(functionA, step * 2.0, vars) / 2.0;
		else
			return easeOut(functionB, (step - 0.5) * 2.0, vars) / 2.0 + 0.5;
	}

	inline double easeOutIn(int functionA, int functionB, double step, EaseVars vars) {
		if (step < 0.5)
			return easeOut(functionA, step * 2.0, vars) / 2.0;
		else
			return easeIn(functionB, (step - 0.5) * 2.0, vars) / 2.0 + 0.5;
	}

	inline double calculateEasingValue(int mode, int functionA, int functionB, double step, EaseVars vars) {
		switch (mode) {
		default:
		case 0:	return easeIn(functionA, step, vars);
		case 1:	return easeOut(functionA, step, vars);
		case 2:	return easeInOut(functionA, functionB, step, vars);
		case 3:	return easeOutIn(functionA, functionB, step, vars);
		}
	}

	inline int StrToEasingMode(const wchar_t* pStr) {
		do {
			if (StrIEqu(pStr, L"easeIn")) {
				return  0;
			}

			if (StrIEqu(pStr, L"easeOut")) {
				return  1;
			}

			if (StrIEqu(pStr, L"easeInOut")) {
				return  2;
			}

			if (StrIEqu(pStr, L"easeOutIn")) {
				return  3;
			}

			return 0;
		} while (false);
	}
}