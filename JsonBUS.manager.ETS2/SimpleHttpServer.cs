using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Web;

namespace JsonBUS.manager
{
    public class SimpleHttpServer
    {

        private HttpListener listener;
        private List<string> prefixes;

        public SimpleHttpServer(string httpPort)
        {
            listener = new HttpListener();
            prefixes = new List<string> { "http://*:" + httpPort + "/" }; // Listen on all assigned addresses on port specified in config file
            foreach (string s in prefixes)
            {
                listener.Prefixes.Add(s);
            }
            listener.Start();
        }

        // Handle incoming HTTP request (used to communicate with navigation and other simulated car devices)
        private async Task HandleIncomingConnections()
        {
            // Note: The GetContext method blocks while waiting for a request. 
            HttpListenerContext context = await listener.GetContextAsync();
            HttpListenerRequest request = context.Request;

            // Obtain a response object.
            HttpListenerResponse response = context.Response;

            // This shit from StackOverflow because somebody invented CORS stupidity (so you can't access one localhost website from other localhost website)
            if (request.HttpMethod == "OPTIONS")
            {
                response.AddHeader("Access-Control-Allow-Headers", "Content-Type, Accept, X-Requested-With");
                response.AddHeader("Access-Control-Allow-Methods", "GET, POST");
                response.AddHeader("Access-Control-Max-Age", "1728000");
            }
            response.AppendHeader("Access-Control-Allow-Origin", "*");

            // Construct a response.
            byte[] buffer = { };
            var fileName = $@"{Environment.CurrentDirectory}\eBolero_web" + request.RawUrl;

            if (File.Exists(fileName))
            {
                response.ContentType = MimeMapping.GetMimeMapping(fileName);
                buffer = File.ReadAllBytes(fileName);
            }
            else
            {
                response.ContentType = "application/json";
                buffer = Encoding.UTF8.GetBytes(ManagerForm.HttpString);
            }


            // Get a response stream and write the response to it.
            response.ContentLength64 = buffer.Length;
            Stream output = response.OutputStream;
            await output.WriteAsync(buffer, 0, buffer.Length);
        }

        public bool IsListening => listener.IsListening;
        public void Stop()
        {
            listener.Stop();
        }

        public async void ListenToHttp(object sender, EventArgs args)
        {
            while (listener.IsListening)
            {
                try
                {
                    await HandleIncomingConnections(); //Non blocking thread
                }
                catch
                {
                }

            }
        }
    }
}
