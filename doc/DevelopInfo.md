Links  
=====  
  
[A Tutorial for GNU libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/tutorial.html#Top)  
[GitHub - libmicrohttpd](https://github.com/ulion/libmicrohttpd)  
  
Using new http features (microhttpd)  
====================================  
  
  If you set http port to 1600

  - [Develop page for testing](http://localhost:1600/devtest)  
  - To get json config info from app, send a get request to http://localhost:1600/info  
      Example result:  

      ```javascript  
		{  
		  "cpu_count" : 4,  
		  "nvidia_list" : [],  
		  "amd_list" : [],  
		  "httpd_port" : 1600,  
		  "pool_address" : "pool.ipbc.io:13333",  
		  "wallet_address" : "someWalletHashAddress..."  
		}  
      ```  

  - To send new config data to the app, send a post request to http://localhost:1600/config  
      Example form data:  

      ```javascript  
		{  
		  "cpu_count" : 1,  
		  "nvidia_list" : false,  
		  "amd_list" : false,  
		  "httpd_port" : 1600,  
		  "pool_address" : "pool.ipbc.io:13333",  
		  "wallet_address" : "someWalletHashAddress..."  
		}  
      ``` 

  - To get json stats info from app, send a get request to http://localhost:1600/api.json  