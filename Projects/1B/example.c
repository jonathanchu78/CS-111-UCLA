#include<zlib.h>


int main(void){
	
	//INITIALIZE
	z_stream in_shell;
	z_stream shell_out;

	in_shell.zalloc = Z_NULL;
	in_shell.zfree = Z_NULL;
	in_shell.opaque = Z_NULL;

	if (deflateInit(&in_shell, Z_DEFAULT_COMPRESSION) != Z_OK){
		fprintf(stderr, "error on deflateInit");
		exit(1);
	}

	shell_out.zalloc = Z_NULL;
	shell_out.zfree = Z_NULL;
	shell_out.opaque = Z_NULL;

	if (inflateInit(&shell_out) != Z_OK){
		fprintf(stderr, "error on inflateInit");
		exit(1);
	}
	
	
	//COMPRESS
	//assume compression buf large enough
	in_shell.avail_in = sizeof(buf); //INPUT BUF HERE
	in_shell.next_in = input_buf;
	in_shell.avail_out = sizeof(compression_buf); //COMPRESSION BUF HERE
	in_shell.next_out = compression_buf;

	//deflate
	do{
		if (deflate(&in_shell, Z_SYNC_FLUSH) != Z_OK){
			fprintf(stderr, "error deflating");
			exit(1);
		}
	} while (in_shell.avail_in > 0);

	
	
	//write
	write(shell_fd, compression_buf, sizeof(compression_buf) - in_shell.avail_out);
	
	
	//DECOMPRESS
	//assume compression buf large enough
	shell_out.avail_in = sizeof(buf); //INPUT BUF HERE
	shell_out.next_in = input_buf;
	shell_out.avail_out = sizeof(compression_buf); //COMPRESSION BUF HERE
	shell_out.next_out = compression_buf;

	//inflate
	do{
		if (inflate(&shell_out, Z_SYNC_FLUSH) != Z_OK){
			fprintf(stderr, "error inflating");
			exit(1);
		}
	} while (shell_out.avail_in > 0);

	
	
	if (inflateEnd(&shell_out) != Z_OK){
		fprintf(stderr, "error on inflate end");
		exit(1);
	}
	
	if (deflateEnd(&in_shell) != Z_OK){
		fprintf(stderr, "error on deflate end");
		exit(1);
	}
}

