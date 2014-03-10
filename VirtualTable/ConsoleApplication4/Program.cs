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
            FileStream fs = File.Open(Console.ReadLine().Trim('\"'), FileMode.Open);
            byte[] b = new byte[fs.Length];
            fs.Read(b, 0, b.Length);
            Console.WriteLine(Encoding.Default.GetString(b));
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
