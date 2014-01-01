using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication4
{

    static class Extension
    {
        public static IEnumerable<String> ReadLines(this TextReader reader)
        {
            String txt = reader.ReadLine();
            while (txt != null)
            {
                yield return txt;
                txt = reader.ReadLine();
            }
        }

        public static int DefaultParse(this String input, int defaultValue)
        {
            int answer;
            return (int.TryParse(input, out answer)) ? answer : defaultValue;
        }
    }


    class Test
    {
        public Test()
        {
            Console.WriteLine(i++);
        }
        private static int i = 1;
    }
    class Program
    {
       
        static void Main(string[] args)
        {
            Action<int> action = null;
            action = x =>
            {
                Console.Write("{0,-4}", x);
                action += r => { return; };
                action.GetInvocationList()[x / 200].DynamicInvoke(x + 1);
            };
            action(1); 
        }
        //             IEnumerable<IEnumerable<int>> rowOfNumbers;
        //             using (TextReader t = new StreamReader(path, Encoding.Default))
        //             {
        //                 rowOfNumbers = ReadNumbersFromStream(t);
        //             }
        // //             var result = (from x in ttt.ParseFile(path)
        //                           select x).Take(10);

        //             foreach (var item in rowOfNumbers)
        //             {
        //                 Console.WriteLine(item);
        //             }
        // //             Console.ReadLine();
        // 
        //             var test = (from x in result
        //                         select x).Take(10);
        //             foreach (var item in test)
        //             {
        //                 Console.WriteLine(item);
        //             }


        //   }
    }
}
