#include <libc.h>
#include <mm.h>

#define NTESTS 8 
#define FPS 70
#define CHILDS 7


void flush_screen(char scen[][80]) {
	for(int i = 0; i < 25; ++i) {
		for(int j = 0; j < 80; ++j) scen[i][j] = '\x00';
	}

}


void init_scen(char scen[][80]) {
	char txt[25] = "Mou-te amb les tecles vim";
	for(int i = 0; i < 25; ++i) scen[3][3+i] = txt[i];
	char	txt2[19] = "Prem 'q' per sortir";
	for(int i = 0; i < 19; ++i) scen[4][3+i] = txt2[i];

	for (int i = 1; i < 24; i++) {
		scen[i][0] = '|';
		scen[i][79] = '|';
	} 
	for (int i = 1; i < 79; i++) {
		scen[0][i] = '-';
		scen[24][i] = '-';
	} 

		scen[0][0] = '+';
		scen[0][79] = '+';
		scen[24][0] = '+';
		scen[24][79] = '+';
}

void omple(char a[], char *b) {

	for(int i = 0; i < strlen(a); ++i) *b++ = a[i];

}
void procrea(int fills) {

	for(int i = 0; i < fills; ++i) {
		int p = fork();
		if(p == 0) {
			while(1);
		}
	
	}

}


int test_fps(int fps, int fills) {
	char scen[25][80];
	flush_screen(scen);
	if( fills > 0) {
		omple("TEST 5: Comprovacio de set_fps() amb diversos procesos\n",&scen[3][3]);
		write(1,&scen[3][3],56);	
	} else {
		omple("TEST 4: Comprovacio de set_fps()\n",&scen[3][3]);
		write(1,&scen[3][3],35);	
	}
	int iter = 0;
	int res = 0;
	procrea(fills);
	while(iter < 6){
		set_fps(fps);
		int max = 1000;
		int i = 0;
		int j = 0;
		int ini = gettime();
		while(i < max){
			j += put_screen((char *)scen);
			++i;
		}
		int spent = gettime()-ini;
		int segs = spent/18;
		segs = (segs*18 < spent)? segs+1 : segs; // Arrodonim cap amunt
		res = (segs == 0)? j : j/segs; 
		res = (res*segs < j)? res+1 : res; // Arrodonim cap amunt 
		if ( res != fps) break; 
		iter += 2;
	}
	set_fps(-1);
	
	if(iter < 6) {

		if( fills > 0) {
			char buff3[] = "TEST 5: FAIL (get:     , expected:     )\n";
			itoa(res , &buff3[19]);
			itoa(fps , &buff3[35]);
			write(1,buff3,42);
		} else {
			char buff3[] = "TEST 4: FAIL (get:     , expected:     )\n";
			itoa(res , &buff3[19]);
			itoa(fps , &buff3[35]);
			write(1,buff3,42);
		}

		flush_screen(scen);
		return 0;
	}
	else {
		if( fills > 0) {
			char buff3[] = "TEST 5: OK (get:     , expected:     )\n";
			itoa(res , &buff3[17]);
			itoa(fps , &buff3[33]);
			write(1,buff3,40);
		} else {
			char buff3[] = "TEST 4: OK (get:     , expected:     )\n";
			itoa(res , &buff3[17]);
			itoa(fps , &buff3[33]);
			write(1,buff3,40);
		}
		flush_screen(scen);
		return 1;

	}
}

int test_max_fps() {
	char scen[25][80];
	flush_screen(scen);
	omple("TEST 3: Comprovacio del maxim de fps amb un unic proces\n",&scen[3][3]);
	write(1,&scen[3][3],strlen(&scen[3][3]));	
	int max = 10000;
	int i = 0;
	int j = 0;
	int ini = gettime();
	while(i < max){
		j += put_screen((char *)scen);
		++i;
	}
	int spent = gettime()-ini;
	int segs = spent/18;
	segs = (segs*18 < spent)? segs+1 : segs; // Arrodonim cap amunt
	int res = (segs == 0)? j : j/segs; 
	res = (res*segs < j)? res+1 : res; // Arrodonim cap amunt 
	char buff[] = "TEST 3: OK (get:     )\n";
	itoa(res , &buff[17]);
	write(1,buff,24);
	return 1;
}
void test_final() {
	fflush();
	char buff[] = "\nTEST 8: provant tot plegat\n";
	write(1,buff,strlen(buff));
	set_fps(70);
	char **scen = (char **)get_scenario();
	flush_screen((char (*)[80])scen);
	init_scen((char (*)[80])scen);
	int x = 12;
	int y = 40;
	char inp = '\x00'; //0xb117ff 
	((char (*)[80])scen)[x][y] = '@';
	while(inp != 'q') {

		get_key(&inp); //0xb11800
		if (inp == 'k') { //0xb117ff
			if(x > 1) {
				--x;
			}
		}

		if (inp == 'l') {
			if(y < 78 ) {
				++y;
			}
		}

		if (inp == 'h') {
			if(y > 1) {
				--y;
			}
		}
		if (inp == 'j') {
			if(x < 23) {
				++x;
			}
		}
	  put_screen((char *)scen);
		del_scenario();
	 	scen = (char **)get_scenario();
		flush_screen((char (*)[80])scen);
		init_scen((char (*)[80])scen);
		((char (*)[80])scen)[x][y] = '@';
		
	}
	char buff1[] = "TEST 8: ? (Creus que va be?) \n";
	write(1,buff1,strlen(buff1));
	flush_screen((char (*)[80])scen);
	put_screen((char *)scen);

}

void test_joc() {
	fflush();
	char buff[] = "\nTEST 1: provant la crida a sistema get_key() i put_screen()\n";
	write(1,buff,strlen(buff));
	char scen[25][80];
	flush_screen(scen);
	init_scen(scen);
	int x = 12;
	int y = 40;
	char inp = 'U'; //0xb117ff 
	scen[x][y] = '@';
	while(inp != 'q') {

		get_key(&inp); //0xb11800
		if (inp == 'k') { //0xb117ff
			if(x > 1) {
				scen[x][y] = '\x00';
				scen[--x][y] = '@';
			}
		}

		if (inp == 'l') {
			if(y < 78 ) {
				scen[x][y] = '\x00';
				scen[x][++y] = '@';
			}
		}

		if (inp == 'h') {
			if(y > 1) {
				scen[x][y] = '\x00';
				scen[x][--y] = '@';
			}
		}
		if (inp == 'j') {
			if(x < 23) {
				scen[x][y] = '\x00';
				scen[++x][y] = '@';
			}
		}
	  put_screen((char *)scen);
		
	}
	char buff1[] = "TEST 1: ? (Creus que va be?) \n";
	write(1,buff1,strlen(buff1));
	flush_screen(scen);
	put_screen((char *)scen);

}

int test_get_key(){
	fflush();
	char buff[] = "TEST 2: provant la crida a sistema get_key()";
	write(1,buff,strlen(buff));
	char buff2[] = "\nEscriu en ordre els seguents caracters: q w e r t y u i o p\n";
	write(1,buff2,strlen(buff2));
	unsigned long t = gettime();
	while(gettime() <= t+18*100);
	char check[8] = "opertyui";
	int i = 0;

	while(i<8){
		char a;
		get_key(&a);
   	if ( a != check[i]) break; 
		++i;
	}	
	if ( i < 8 ) { // No passa el test
		char buff3[] = "TEST 2: FAIL (segur que has premut els caracters indicats?)\n";
		write(1,buff3,strlen(buff3));
		return 0;

	} else // passa el test
	{

		char buff3[] = "TEST 2: OK\n";
		write(1,buff3,strlen(buff3));
		return 1;
	}
}

int test_fflush(){
	char **neteja = (char **) get_scenario();
	flush_screen((char (*)[80])neteja);
	put_screen((char *)neteja);
	del_scenario();		
	char buff[] = "TEST 6: provant la crida a sistema fflush()";
	write(1,buff,strlen(buff));
	char buff2[] = "\nEscriu alguns caracters si us plau.\n";
	write(1,buff2,strlen(buff2));
	unsigned long t = gettime();
	while(gettime() <= t+18*90);
	fflush();
	char a;
	get_key(&a);
	if (a != '\x00'){
		char buff3[] = "TEST 6: FAIL\n";
		write(1,buff3,strlen(buff3));
		return 0;
	} else {
		char buff3[] = "TEST 6: OK\n";
		write(1,buff3,strlen(buff3));
		return 1;
	}
}

int test_sbrk(){	
	char buff[] = "TEST 7: provant la crida a sistema sbrk()\n";
	write(1,buff,strlen(buff));
	unsigned long a = (unsigned long)sbrk(-4); // ha de retornar error: -1
	if (a != -1 || (unsigned long)sbrk(0) != L_HEAP_START){	
		char buff1[] = "TEST 7: FAIL\n";
		write(1,buff1,strlen(buff1));
		return 0;
	}
	unsigned int brk = (unsigned long)sbrk(200);
	a = (unsigned long)sbrk(0);
	if (brk+200 != a){
		char buff2[] = "TEST 7: FAIL\n";
		write(1,buff2,strlen(buff2));
		return 0;
	}
	sbrk(-2000);
	a = (unsigned long)sbrk(0);
	if (a != L_HEAP_START){	
		char buff3[] = "TEST 7: FAIL\n";
		write(1,buff3,strlen(buff3));
		return 0;
	}
		char buff4[] = "TEST 7: OK\n";
		write(1,buff4,strlen(buff4));
		return 1;
	
}

void jp_all() {
	int total = NTESTS;
	int failed = total; 
	int unknown = 2;


	test_joc(); // 1 unkown
	--failed;
	failed -= test_get_key(); //2
	failed -= test_max_fps();//3
	failed -= test_fps(FPS, 0); // 4
	failed -= test_fps(FPS, CHILDS); // 5
	failed -= test_fflush(); //6
	failed -= test_sbrk(); //7
	test_final(); // 8 unkown
	--failed;



	char res[] = "\n\nFAILED TESTS:   \nUNKNOWN RESULTS:   \nTOTAL:   \n";
	itoa(failed, &res[16]);
	itoa(unknown, &res[36]);
	itoa(total, &res[46]);
	write(1, res,48);
}

void jp_rank(int ini, int fin){
	int unknown = 0;
	int total = fin - ini +1;
	int failed = total; 
	for(int i = ini ; i <= fin; i++) {

		switch(i) {
			case 1:
				test_joc();
				++unknown;
				--failed;
				break;
			case 2:
				failed -= test_get_key();
				break;
			case 3:
				failed -= test_max_fps();
				break;
			case 4:
				failed -= test_fps(FPS, 0);
				break;
			case 5:
				failed -= test_fps(FPS, CHILDS);
				break;
			case 6:
				failed -= test_fflush();
				break;
			case 7:
				failed -= test_sbrk();
				break;
			case 8:
				test_final(); //  unkown
				++unknown;
				--failed;
				break;

		}


	} 
	char scen[25][80];
	flush_screen(scen);
	put_screen((char *)scen);
	char res[] = "\n\nFAILED TESTS:   \nUNKNOWN RESULTS:   \nTOTAL:   \n";
	itoa(failed, &res[16]);
	itoa(unknown, &res[36]);
	itoa(total, &res[46]);
	write(1, res,48);

}
