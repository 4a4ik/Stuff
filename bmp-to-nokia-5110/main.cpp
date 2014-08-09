#include <iostream>
#include <fstream> 
#include <string> 

using namespace std;

int main(int argc, char *argv[])
{
	

	int size = 0, pixels_adress = 0, width = 0, height = 0;
	short int bits_per_pixel = 0;

	if (argc >= 2 )
		cout << "Your file is: " << argv[1] << endl;

	else
	{
		cout << "Please drag and drop .bmp file on the .exe" << endl;
		system("PAUSE");
		return 0;
	}


	ifstream file (argv[1], ios::in | ios::binary);

	file.seekg( 2, ios::beg);

	file.read ((char*)&size, sizeof(int));
	////////////////////////////////////////////////////

	file.seekg( 10, ios::beg);

	file.read ((char*)&pixels_adress, sizeof(int));
	//////////////////////////////////////////////////////

	file.seekg( 18, ios::beg);

	file.read ((char*)&width, sizeof(int));

	file.read ((char*)&height, sizeof(int));
	/////////////////////////////////////////////////////

	if (width != 84 && height != 48)
	{
		cout << "Sorry, but your image needs to be 84x48 pixels" << endl;
		system("PAUSE");
		return 0;
	}

	file.seekg( 28, ios::beg);

	file.read ((char*)&bits_per_pixel, sizeof(short int));

	/////////////////////////////////////////////////////////

	file.seekg( pixels_adress, ios::beg);



	cout << "Size: " << size << endl;
	cout << "pixels_adress: " << pixels_adress << endl;
	cout << "bits per pixel: " << bits_per_pixel << endl;
	cout << "Width: " << width << endl;
	cout << "Height: " << height << endl;

	char map[84][48];

	///////////////////// 1 BIT
	if( bits_per_pixel == 1 )
	{
		unsigned char bgr;
		
		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{
				file.read ((char*)&bgr, 1);

				if ( x >= 80 )
				{
					for ( int n = 7; n >= 4; n-- )
					{
						if ( bgr & ( 1 << n ) ) // if 1 bit of readed pixels and 0b10000000
							map[x][y] = '0';
						else
							map[x][y] = '1';

						if ( n != 4 ) x++;
					}
				}

				else
				{
					for ( int n = 7; n >= 0; n-- )
					{
						if ( bgr & ( 1 << n ) ) // if 1 bit of readed pixels and 0b10000000
							map[x][y] = '0';
						else
							map[x][y] = '1';

						if ( n != 0 ) x++;
					}
				}
			}
			file.read ((char*)&bgr, 1); // 12 bits offset
		}
	}
	//////////////// 1 bit END
	///////////////////// 4 BIT
	else if( bits_per_pixel == 4 )
	{
		unsigned char bgr;

		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{
				
				file.read ((char*)&bgr, 1);
				
				if ( bgr & 0xF0 ) 
					map[x][y] = '0';
				else
					map[x][y] = '1';

				x++;

				if ( bgr & 0x0F )
					map[x][y] = '0';
				else
					map[x][y] = '1';
			}
			file.read ((char*)&bgr, 1); // 2 offset bytes
			file.read ((char*)&bgr, 1);
		}
	}
	//////////////// 4 bit END
	///////////////////// 8 BIT
	else if( bits_per_pixel == 8 )
	{
		unsigned char bgr;

		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{

				file.read ((char*)&bgr, 1);

				if ( bgr == 0xFF )
					map[x][y] = '0';
				else
					map[x][y] = '1';
			}
		}
	}
	//////////////// 8 bit END
	///////////////////// 16 BIT
	else if( bits_per_pixel == 16 )
	{
		unsigned short int bgr;

		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{
				file.read ((char*)&bgr, 2);

				if ( bgr >= 0xFFF ) 
					map[x][y] = '0';
				else
					map[x][y] = '1';
			}
		}
	}
	//////////////// 16 bit END
	///////////////////// 24 BIT
	else if( bits_per_pixel == 24 )
	{
		unsigned int bgr = 0;

		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{
				file.read ((char*)&bgr, 3);

				//cout << bgr << endl;

				if ( bgr == 0xFFFFFF ) 
					map[x][y] = '0';
				else
					map[x][y] = '1';

				bgr = 0;
			}
		}
	}
	//////////////// 24 bit END
	///////////////////// 32 BIT
	else if( bits_per_pixel == 32 )
	{
		unsigned int bgr;

		for( int y = 47; y >= 0; y-- )
		{
			for( int x = 0; x < 84; x++ )
			{
				file.read ((char*)&bgr, 4);

				if ( bgr >= 0xFFFFFF ) 
					map[x][y] = '0';
				else
					map[x][y] = '1';
			}
		}
	}
	//////////////// 32 bit END
	else
	{
		cout << "Sorry, but your image needs to have 1, 4, 16, 24 or 32 bits per pixel" << endl;
		system("PAUSE");
		return 0;
	}
	

	file.close();

	///////////CREATE VERTICAL file

	unsigned int pixel_map[84][6];
	ofstream out("vertical.h");

	out << "/* File created with \"BMP to nokia 5110 LCD\"" << endl;
	out << "* Vertical addressing mode" << endl;
	out << "* Programm created by 4a4ik" << endl;
	out << "* echachik.blogspot.com" << endl;
	out << "*/" << endl << endl;

	out << "const unsigned char img [504] = {" << endl;

	for( int x = 0; x < 84; x++ )
	{
		if ( !(x % 2) ) out << "\t";  // horizontal tab

		for( int y = 0; y < 6; y++ )
		{
			pixel_map[x][y] = 0;
			for( int i = 0; i < 8; i++ )
			{
				if ( map[x][y * 8 + i] == '1' )
					pixel_map[x][y] += ( 1 << i );
			}
			out << "0x" ;
			if ( pixel_map[x][y] < 16 )		out << "0";
			out << hex  << uppercase << pixel_map[x][y] << nouppercase << dec << ", ";
		}
		if ( x % 2 ) out << endl;
	}

	out << "};" << endl;

	out.close();

	///////////////////////////
	///////////CREATE HORIZONTAL file

	out.open("horizontal.h");

	out << "/* File created with \"BMP to nokia 5110 LCD\"" << endl;
	out << "* Horizontal addressing mode" << endl;
	out << "* Programm created by 4a4ik" << endl;
	out << "* echachik.blogspot.com" << endl;
	out << "*/" << endl << endl;

	out << "const unsigned char img [504] = {";

	for( int y = 0; y < 6; y++ )
	{
		for( int x = 0; x < 84; x++ )
		{
			if( !(( x + y * 84 ) % 12) ) out << endl << "\t";  // horizontal tab
			out << "0x" ;
			if ( pixel_map[x][y] < 16 )		out << "0";
			out << hex  << uppercase << pixel_map[x][y] << nouppercase << dec << ", ";
		}
	}

	out << endl << "};" << endl;

	out.close();
	
	///////////////////////////

	system("PAUSE");
	return 0;
}
