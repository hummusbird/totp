using System.Text;
using System.Security.Cryptography;
namespace totp;

class Program
{
    static void Main(string[] args)
    {
        string secret = "12345678901234567890"; // enter secret as a string.

        while (true)
        {
            long unix = DateTimeOffset.UtcNow.ToUnixTimeSeconds(); // get the current UTC timestamp
            Console.WriteLine("unix: " + unix);
            Console.WriteLine("time: " + DateTimeOffset.FromUnixTimeSeconds(unix).DateTime);
            Console.WriteLine("step: " + DateTimeOffset.FromUnixTimeSeconds((unix / 30) * 30).DateTime); // current 30s timestep
            Console.WriteLine("totp: " + generateTOTP(secret, unix / 30, 6));
            Console.WriteLine("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            Thread.Sleep(1000);
        }
    }

    static string generateTOTP(string secret, long time, int digits)
    {
        string result = "";

        // uncommenting the TOTP line will use the byte array below as the msg, instead of the current unix timestamp.
        // remember to comment the TOTP line and endian flip

        //byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 2 }; // HOTP
        byte[] msg = BitConverter.GetBytes(time); // TOTP
        if (BitConverter.IsLittleEndian) { Array.Reverse(msg); }

        byte[] key = Encoding.ASCII.GetBytes(secret); // Encode the secret as bytes

        HMACSHA1 hmac_sha = new HMACSHA1(key);
        byte[] hash = hmac_sha.ComputeHash(msg); // compute the hash using the key and msg

        Console.WriteLine("key : " + BitConverter.ToString(key).Replace("-", ""));
        Console.WriteLine("msg : " + BitConverter.ToString(msg).Replace("-", ""));
        Console.WriteLine("hash: " + BitConverter.ToString(hash).Replace("-", ""));

        // truncate the hash to generate the OTP
        int offset = hash[^1] & 0x0f;
        int bin_code = (hash[offset] & 0x7f) << 24
               | (hash[offset + 1] & 0xff) << 16
               | (hash[offset + 2] & 0xff) << 8
               | (hash[offset + 3] & 0xff);

        result = (bin_code % Math.Pow(10, digits)).ToString(); // trim to N digits

        while (result.Length < digits) { result = "0" + result; } // pad with zeros

        return result;
    }
}