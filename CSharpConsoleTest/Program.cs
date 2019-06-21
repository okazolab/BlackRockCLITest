using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSharpConsoleTest
    {
    class Program
        {
        static void Main(string[] args)
            {
            BlackRockEngine BREngine = new BlackRockEngine(0);
            BREngine.Start();
            if (BREngine.StartAcquisition(5))
                {                
                Console.WriteLine("Press any key to stop"); 
                while (!Console.KeyAvailable)
                    {                    
                    double[] Data = BREngine.GetData();
                    if (Data.Length>0)
                        Console.WriteLine("First sample: " + Data[0]);                    
                    }
                }
            BREngine.Stop();
            Console.ReadKey();
            }
        }
    }
