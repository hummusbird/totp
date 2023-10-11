using System.Text;
using System.Security.Cryptography;
namespace totp;

class Program
{
    static void Main(string[] args)
    {
        string secret = "12345678901234567890";

        while (true)
        {
            long unix = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
            Console.WriteLine("unix: " + unix);
            Console.WriteLine("time: " + DateTimeOffset.FromUnixTimeSeconds(unix).DateTime);
            Console.WriteLine("step: " + DateTimeOffset.FromUnixTimeSeconds((unix / 30) * 30).DateTime);
            Console.WriteLine("totp: " + generateTOTP(secret, unix / 30, 8));
            Console.WriteLine("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            Thread.Sleep(1000);
        }
    }

    static string generateTOTP(string secret, long time, int digits)
    {
        string result = "";

        //byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 2 }; // HOTP
        byte[] msg = BitConverter.GetBytes(time); // TOTP
        if (BitConverter.IsLittleEndian) { Array.Reverse(msg); }

        byte[] key = Encoding.ASCII.GetBytes(secret);
        //if (BitConverter.IsLittleEndian) { Array.Reverse(key); }

        HMACSHA1 hmac_sha = new HMACSHA1(key);
        byte[] hash = hmac_sha.ComputeHash(msg);
        //if (BitConverter.IsLittleEndian) { Array.Reverse(hash); }

        Console.WriteLine("hash: " + BitConverter.ToString(hash).Replace("-", ""));
        Console.WriteLine("key : " + BitConverter.ToString(key).Replace("-", ""));
        Console.WriteLine("msg : " + BitConverter.ToString(msg).Replace("-", ""));

        int offset = hash[^1] & 0x0f;
        int bin_code = (hash[offset] & 0x7f) << 24
               | (hash[offset + 1] & 0xff) << 16
               | (hash[offset + 2] & 0xff) << 8
               | (hash[offset + 3] & 0xff);

        result = (bin_code % Math.Pow(10, digits)).ToString();

        while (result.Length < digits) { result = "0" + result; }

        return result;
    }

    static public IEnumerable<byte> GetBytesFromByteString(string s)
    {
        for (int index = 0; index < s.Length; index += 2)
        {
            yield return Convert.ToByte(s.Substring(index, 2), 16);
        }
    }
}