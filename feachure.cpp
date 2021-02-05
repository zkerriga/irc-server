#include <string.h>
#include <stdio.h>
#include <string>

/*
* Include mbedtls
*/
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#define SERVER_PORT "443"
#define SERVER_NAME "www.google.com"
#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

#define DEBUG_LEVEL 1

void my_debug(void *ctx, int level, const char *str)
{
	if (level < DEBUG_LEVEL)
	{
		fprintf((FILE *)ctx, "%s", str);
		fflush((FILE *)ctx);
	}
}

int main(void)
{
	int ret = 0, len;
	unsigned char buf[1024];
	mbedtls_net_context server_fd;

	/*
	* Init mbedtls
	*/
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	std::string pers("mbedtls_ssl_example");
//	char *pers = "mbedtls_ssl_example";

	mbedtls_net_init( &server_fd );
	mbedtls_ctr_drbg_init( &ctr_drbg );

	/*
	* Seed the Random Number Generator
	*/
	printf( "\n  . Seeding the random number generator..." );
	fflush( stdout );

	mbedtls_entropy_init( &entropy );
	if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
									   ( const unsigned char * )pers.c_str(),
									   pers.size() ) ) != 0)
	{
		printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
		fflush( stdout );
		goto exit;
	}

	/*
	* Start the connection
	*/
	printf( "\n  . Connecting to tcp/%s/%s...", SERVER_NAME, SERVER_PORT );
	fflush( stdout );

	/*
	* Connect with mbedtls
	*/
	if( ( ret = mbedtls_net_connect( &server_fd, SERVER_NAME,
									 SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
	{
		printf( " failed\n  ! net_connect returned %d\n\n", ret );
		fflush( stdout );
		goto exit;
	}

	/*
	* Initialize the SSL context
	*/
	mbedtls_ssl_init( &ssl );
	mbedtls_ssl_config_init( &conf );
	printf( " ok\n" );

	printf("  . Setting up the SSL/TLS structure...");
	fflush( stdout );

	if( ( ret = mbedtls_ssl_config_defaults( &conf,
											 MBEDTLS_SSL_IS_CLIENT,
											 MBEDTLS_SSL_TRANSPORT_STREAM,
											 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
	{
		printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
		fflush( stdout );
		goto exit;
	}

	mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_NONE );

	mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
	mbedtls_ssl_conf_dbg(&conf,
						 reinterpret_cast<void (*)(void *, int, const char *,
												   int,
												   const char *)>(my_debug), stdout );
	mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

	if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
	{
		printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
		fflush( stdout );
		goto exit;
	}
	printf( " ok\n" );

	/*
	* Write the GET request
	*/
	printf( "  > Write to server:" );
	fflush(stdout);

	len = snprintf((char *)buf, sizeof(buf), GET_REQUEST);

	while( ( ret = mbedtls_ssl_write(&ssl, buf, (size_t)len ) ) <= 0 )
	{
		if( ret != 0 )
		{
			printf( " failed\n  ! write returned %d\n\n", ret );
			fflush( stdout );
			goto exit;
		}
	}

	len = ret;
	printf( " %d bytes written\n\n%s", len, ( char * )buf );

	/*
	* Read the HTTP response
	*/
	printf( "  < Read from server:" );
	fflush( stdout );
	do
	{
		len = sizeof( buf ) - 1;
		memset( buf, 0, sizeof( buf ) );
		ret = mbedtls_ssl_read( &ssl, buf, (size_t)len );
		if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
			continue;

		if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
			break;

		if( ret < 0 )
		{
			printf( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret );
			fflush( stdout );
			break;
		}

		if( ret == 0 )
		{
			printf( "\n\nEOF\n\n" );
			fflush( stdout );
			break;
		}

		len = ret;
		printf(" %d bytes read\n\n%s", len, ( char *)buf );
	} while( 1 );
	mbedtls_ssl_close_notify( &ssl );

	exit:

	mbedtls_net_free( &server_fd );
	mbedtls_ssl_free( &ssl );
	mbedtls_ssl_config_free( &conf );
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );
	memset( &ssl, 0, sizeof( ssl ) );

#ifdef _WIN32
	printf( "  + Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

	return( ret );
}