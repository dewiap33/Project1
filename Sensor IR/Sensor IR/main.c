#define F_CPU 8000000UL /* Berfungsi untuk mendefinisikan frekuensi crystal yang digunakan sebesar 8 MHz */
#include <avr/io.h>  /* Berfungsi untuk memasukkan library untuk mengenali fungsi input output pada AVR*/
#include <util/delay.h>  /* Berfungsi untuk memasukkan library untuk fungsi delay*/
#define LED_OUTPUT		PORTD // menyatakan PORTD sebagai output led
#define IR_Input		PINC // menyatakan pinc sebagai input IR
#include <stdio.h>	/* Berfungsi untuk memasukkan library mendeklarasikan fasilitas I/O
standar yang diterapkan di avr-libc, yang akan digunakan pada program
ini untuk fungsi sprint().*/
#include <stdlib.h>  /* memasukkan library untuk operasi pembanding*/
#define LCD_Dir  DDRB		 /* Mendefinisikan data port direction untuk LCD*/
#define LCD_Port PORTB		/* data port untuk LCD*/
#define RS PB0			/*Mendefinisikan PINB0 sebagai register untuk RS pada LCD */
#define EN PB1 				/* Mendefinisikan Enable signal pada LCD di PINB1 */


void LCD_Command( unsigned char cmnd ){
	/* Mendefinisikan fungsi untuk mengatur LCD */
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
	/* mengirim empat bit pertama*/
	LCD_Port &= ~ (1<<RS);
	/* memiliki arti bahwa bit yang dikontrol oleh pin RS pada
	LCD_Port akan diatur menjadi 0 menunjukkan bahwa perintah
	yang akan dikirim ke layar LCD adalah perintah (command) dan
	bukan data*/
	LCD_Port |= (1<<EN);
	/* memiliki arti bahwa bit yang dikontrol oleh pin EN (Enable)
	pada LCD_Port akan diatur menjadi 1 untuk membangkitkan pulsa
	Enable. */
	_delay_us(1);
	/* melakukan delay sesaat */
	LCD_Port &= ~ (1<<EN);
	/* melakukan delay sesaat memiliki arti bahwa bit yang
	dikontrol oleh pin EN (Enable) pada LCD_Port akan diatur
	menjadi 0 sehingga menghentikan pulsa Enable. */
	_delay_us(200);
	/* melakukan delay sesaat */
	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
	/* arti bahwa byte yang dikontrol oleh LCD_Port akan
	dimodifikasi untuk mengirimkan empat bit paling rendah (LSB -
	Least Significant Bits) dari cmnd ke layar LCD.*/
	LCD_Port |= (1<<EN);
	_delay_us(10);
	/* Pernyataan ini mengatur bit yang dikendalikan oleh pin EN
	(Enable) pada LCD_Port menjadi 1 mengaktifkan pulsa Enable. Ini
	digunakan untuk memberikan sinyal yang memicu eksekusi
	perintah atau penulisan data ke layar LCD. */
	LCD_Port &= ~ (1<<EN);
	_delay_ms(10);
	/* melakukan delay sesaat Pernyataan ini mengatur bit yang
	dikendalikan oleh pin EN (Enable) pada LCD_Port menjadi 0
	(logika rendah), menghentikan pulsa Enable. Ini menonaktifkan
	pulsa Enable setelah penundaan selesai. */
}

void LCD_Char( unsigned char data ){
	/* merupakan fungsi untuk mengirimkan sebuah karakter (data)
	ke layar LCD dalam mode 4-bit */
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0);
	/* mengirim empat bit utama dari data ke layar LCD dengan
	memodifikasi empat bit paling utama dari LCD_Port.*/
	LCD_Port |= (1<<RS);
	/* Pernyataan ini mengatur bit yang dikendalikan oleh pin RS
	pada LCD_Port menjadi 1, memberi tahu bahwa data yang akan
	dikirimkan ke layar LCD merupakan data dan bukan command.*/
	LCD_Port|= (1<<EN);
	/* mengatur bit yang dikendalikan oleh pin EN pada LCD_Port
	menjadi 1, mengaktifkan pulsa Enable untuk mengirim data ke
	layar LCD. */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	/* mengatur bit yang dikendalikan oleh pin EN pada LCD_Port
	menjadi 0 sehingga menghentikan pulsa Enable.*/
	_delay_us(200);
	LCD_Port = (LCD_Port & 0x0F) | (data << 4);
	/* ini mengirim empat bit terakhir dari data ke layar LCD
	dengan memodifikasi empat bit terakhir */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	/* mengatur bit yang dikendalikan oleh pin EN (Enable) pada
	LCD_Port menjadi 1 sehingga mengaktifkan pulsa Enable untuk
	mengirim data ke layar LCD.*/
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
	/* mengatur bit yang dikendalikan oleh pin EN (Enable) pada
	LCD_Port menjadi 0 untuk menghentikan pulsa Enable.*/
}

void LCD_Init (void){	/* mengatur inisialisasi awal pada LCD.*/
	LCD_Dir = 0xFF;			/* Mengatur arah port LCD sebagai output*/
	_delay_ms(20);			
	
	LCD_Command(0x02);		/* perintah 0x02 untuk menginisialisasi LCD dalam mode 4 bit*/
	LCD_Command(0x28);      /* perintah 0x28 untuk mengatur mode 2 baris dengan matriks 5x7 karakter dalam mode 4 bit*/
	LCD_Command(0x0c);     /* Menampilkan teks pada LCD tanpa kursor*/
	LCD_Command(0x06);  /* perinta 0x06 untuk mengaktifkan penambahan otomatis posisi kursor (geser kursor kekanan)setiap kali data dikirim*/
	LCD_Command(0x01);     /* membersihkan tampilan LCD*/
	_delay_ms(2);
}

void LCD_String (char *str){		
/* funsi ini akan mengirimkan karakter satu persatu dari string yang
diberikan hingga mencapai karater null(‘\o’),setiap karakter dikirin
menggunakan fungsi LCD_Char*/
	int i;
	for(i=0;str[i]!=0;i++)		
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)	
{
	/* Digunakan untuk mengirimkan LCD dengan posisi (baris dan kolom)
	yang ditentukan*/
	
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	
	/* Menentukan jika parameter row (baris) =0 dan parameter pos (kolom)
	menentukan baris 0-15*/
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	
	/* Menentukan jika parameter row (baris) =0 dan parameter pos (kolom)
	menentukan baris 0-15*/
	LCD_String(str);		
	/* memanggil fungsi LCD_String untuk mengirimkan string ke LCD*/

}

void LCD_Clear()
{
	/* Digunakan Untuk membersihkan tampilan LCD*/
	LCD_Command (0x01);		
	_delay_ms(2);
	LCD_Command (0x80);		
}



int main(void){
	int jumlah;
	uint8_t benda = 0;
	char buffer[20];
	
	DDRC = 0x00;	/* Set IR */
	DDRD = 0xff;	/* Set LED */
	
	LCD_Init();
	LCD_String_xy(0,0, "INFO");
	
	while(1){
		
		LED_OUTPUT = IR_Input; // output dari led sesuai dengan nilai input dari IR
		
		if ( PINC & (1<<0)){ // kondisi ketika IR aktif
			benda += 1; // menambahkan variabel benda dengan jumlah 1 setiap IR aktif
			jumlah = benda; // memberikan nilai variabel jumlah sesuai dengan variabel benda

			sprintf(buffer, "Barang = %d       ", jumlah); // mengubah variabel jumlah (int) menjadi char 
			LCD_String_xy(1, 0, buffer); // menampilkan hasil jumlah
			_delay_ms(200);
			
		}

	}
}