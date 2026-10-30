#define READ 0
#define WRITE 1

#define SIZEofBUFF 20
#define SSizeofBUFF 6

typedef struct{
	int  	custid;
	char 	LastName[SIZEofBUFF];
	char 	FirstName[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
} MyRecord;