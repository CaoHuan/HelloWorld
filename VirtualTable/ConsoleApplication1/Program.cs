using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.IO;

namespace ConsoleApplication1
{
    struct OurResult
    {
        public int i;
    }

    interface IOurInterface
    {
        OurResult GetResult();
    }

    class OurTest : IOurInterface
    {

        public OurResult GetResult()
        {
            OurResult or;
            or.i = 100;
            return or;
        }
    }

    class Test
    {
        public Test()
        {

            Console.WriteLine(i++);
        }

        int i = 0;
    }

    class Program
    {
        static void Main(string[] args)
        {

//             Parallel.For(1, 10000, (num) =>
//             {


                Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPEndPoint ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 6000);
                socket.Connect(ep);
                byte[] b = new byte[100];
                //                                  socket.Receive(b);
                //                                  Console.WriteLine(Encoding.ASCII.GetString(b));
                int i = 0;

                while (true)
                {
                    b = Encoding.ASCII.GetBytes("This is a test...  " + i.ToString() + Environment.NewLine);
                    int res = socket.Send(b);

                    //Console.WriteLine(i++);
                    if (i == 100000)
                    {
                        break;
                    }
                    ++i;
                }

                while (true)
                {
                    int size = socket.ReceiveBufferSize;
                    b = new byte[1024 * 80];
                    socket.Receive(b);
                    File.AppendAllText("TestThread" /*+ AppDomain.GetCurrentThreadId().ToString() + "NUM " + num.ToString()*/ + ".txt", Encoding.ASCII.GetString(b));
                }
 //           });

// 
//             while (true)
//             {
//                 int size = socket.ReceiveBufferSize;
//                 b = new byte[1024 * 80];
//                 socket.Receive(b);
//                 Console.WriteLine(Encoding.ASCII.GetString(b));
//                 Console.ReadLine();
//             }
            

            
        }



        public static T Max<T>(IList<T> list, Int32 right, Int32 left) where T : IComparable<T>
        {
            /************************************************************************
           
            
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 6000);
            socket.Connect(ep);
            while (true)
            {
                byte[] b = new byte[100];
                socket.Receive(b);
                Console.WriteLine(Encoding.ASCII.GetString(b));

                Console.WriteLine(socket.LocalEndPoint.ToString());
                string str = Console.ReadLine();
                socket.Send(Encoding.ASCII.GetBytes(str));


            }
            /************************************************************************/
            //做一些比较操作之类的等等
            int max = right;
            while (right <= left)
            {
                if (list[max].CompareTo(list[right++]) < 0)
                {
                    max = left;
                }

            }
            return list[max];
        }
    }
}
