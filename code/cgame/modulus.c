#include <stdio.h>
#include <math.h>

#define PI 3.1415926535897932384

int main( int argc, char *argv[] ) 
{
	double err, divide, e0;
	double v0, v1;
	int mod, n, times;

	if ( argc < 2 ) 
	{
		divide = 1000; // default value
	}
	else 
	{
		divide = atof( argv[1] );
		if ( divide == 0 )
			return 0;
	}

	printf( "=======\ncalculating best modulus for %f divider\n=======\n", divide );

	e0 = 10000000000;

	for ( n = 1; ; n++ ) 
	{

		mod = (int)( (double) n * divide * 2.0 * PI );
		if ( mod > 16*1024*1024 )
			break;

		times = (0x7FFFFFFF / mod) + 1; // times we should multiply phase error

		v0 = (double) n * divide* 2.0 * PI;
		v1 = mod;

//		err = sin( v0 - v1 ) * (double) times / (2.0 * PI ) * 100.0;
		err = fabs( v0 - v1 ) * (double) times / (2.0 * PI ) * 100.0;

		if ( err <= e0 ) 
		{
			e0 = err;
			printf( "modulus:%8i phase error:%13.2f%%\n", mod, err );
		}
	}

	printf( "=======\nfinished\n" );

	return 0;
}
