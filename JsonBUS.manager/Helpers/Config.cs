using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace JsonBUS.manager.Helpers
{
    public class Config
    {
        private readonly JObject configObj;

        public Config()
        {
            string configPath = $@"{Environment.CurrentDirectory}\_config.json";
            configObj = JObject.Parse(File.ReadAllText(configPath));
        }

        public bool HttpListen => (bool)configObj["httpListen"];

        public string HttpPort => (string)configObj["httpListenPort"];

        public IList<string> SerialPorts
        {
            get
            {
                return configObj["serialPorts"].Select(c => (string)c).ToList();
            }
        }
    }
}
