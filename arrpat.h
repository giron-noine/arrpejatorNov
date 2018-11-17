#ifndef _ARRPAT_H_
#define _ARRPAT_H_

	//protetype
	typedef struct {
	int	patt;
	int scale;
	int note;
	} arrpInfo_t;

	typedef struct {
	int stepNum;
	int noteNum;
	} arrpNote_t;

	arrpNote_t arrpNote noiPatt(arrpInfo_t arrpInfo);

#endif // _ARRPAT_H_