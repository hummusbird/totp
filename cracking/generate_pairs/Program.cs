using System.Text;
using System.Security.Cryptography;
namespace generate_pairs;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Enter secret: ");
        string secret = Console.ReadLine();
        Console.WriteLine("Enter starting timestamp: ");
        long unix = long.Parse(Console.ReadLine());
        Console.WriteLine("Enter amount of pairs: ");
        long pairs = long.Parse(Console.ReadLine());

        for (int i=0; i < pairs; i++)
        {
            string output = generateTOTP(secret, (unix + i * 30) / 30, 6) + ":" + (unix + i * 30);

            Console.WriteLine(output);

            File.AppendAllText("totp_pairs.hash", output + Environment.NewLine);
        }
    }

    static string generateTOTP(string secret, long time, int digits)
    {
        byte[] msg = BitConverter.GetBytes(time); // TOTP
        if (BitConverter.IsLittleEndian) { Array.Reverse(msg); }

        byte[] key = Encoding.ASCII.GetBytes(secret); // Encode the secret as bytes

        HMACSHA1 hmac_sha = new HMACSHA1(key);
        byte[] hash = hmac_sha.ComputeHash(msg); // compute the hash using the key and msg

        // truncate the hash to generate the OTP
        int offset = hash[^1] & 0x0f;
        int bin_code = (hash[offset] & 0x7f) << 24
               | (hash[offset + 1] & 0xff) << 16
               | (hash[offset + 3] & 0xff);

        string result = (bin_code % Math.Pow(10, digits)).ToString(); // trim to N digits

        while (result.Length < digits) { result = "0" + result; } // pad with zeros

        return result;
    }
}
