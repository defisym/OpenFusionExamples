#pragma once

//Check if a dir has animation
inline bool DirHasAnimation(LPRDATA rdPtr, LPRO object, size_t Dir) {
	Dir = max(0, min(DIRID_MAX - 1, Dir));

	if (object == NULL) {
		return false;
	}

	return (object->roa.raAnimOffset->anOffsetToDir[Dir] > 0) ? true : false;
}

inline bool DirHasAnimation(LPRDATA rdPtr, int Fixed, size_t Dir) {	
	return DirHasAnimation(rdPtr, LproFromFixed(rdPtr, Fixed), Dir);
}

#define DirHasAnimation(X, Dir) DirHasAnimation(rdPtr, X, Dir)

//Get object's display animation direction
inline size_t DisplayAnimationDirection(LPRDATA rdPtr, LPRO object) {
	if (object == NULL) {
		return 0;
	}

	//Mechanism
	//fusion decides to display a replaced animation when current direction doesn't have one by following rules below:
	//	- Display the closest direction(0 is treated as 32)
	//	- If both former and later have the same distance, display the one backward to the direction of direction change(if only 0 and 8 have animation, switch from 3->4, display 0, switch from 5->4, display 8)

	size_t former = 0;
	size_t later = 0;

	size_t curdir = object->roa.raAnimDir;
	size_t prevdir = object->roa.raAnimPreviousDir;

	bool clockwize;

	if (prevdir == 0) {
		if (curdir <= 16) {
			clockwize = false;
		}
		else {
			clockwize = true;
		}
	}
	else {
		clockwize = prevdir >= curdir;
	}

	//former
	for (size_t pos = curdir; pos >= 0; pos--) {
		if (DirHasAnimation(object, pos)) {
			former = pos;
			break;
		}
	}

	//later
	for (size_t pos = curdir; pos <= DIRID_MAX; pos++) {
		if (DirHasAnimation(object, pos % 32)) {
			later = pos;
			break;
		}
	}

	if ((curdir - former) == (later - curdir)) {
		return !clockwize ? former : later % 32;
	}
	else {
		return (curdir - former) <= (later - curdir) ? former : later % 32;
	}
}

inline size_t DisplayAnimationDirection(LPRDATA rdPtr, int Fixed) {
	return DisplayAnimationDirection(rdPtr, LproFromFixed(rdPtr, Fixed));
}

#define DisplayAnimationDirection(X) DisplayAnimationDirection(rdPtr, X)
