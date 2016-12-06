// Include the Phant library:
#include "SparkFunPhant.h":

const char server[] = "data.sparkfun.com"; // Phant destination server
const char publicKey[] = "0lqbbKWjqvuqNovxX379"; // Phant public key
const char privateKey[] = "D6MRRY7eMAcYq0AxyeMW"; // Phant private key
Phant phant(server, publicKey, privateKey); // Create a Phant object

phant.add("barotemp", baroTemp);
phant.add("humidity", humidity);
phant.add("rainin", rainin);
phant.add("tempf", tempf);
phant.add("winddir", winddir);
phant.add("windspeedmph", windspeedmph);

TCPClient client;
if (client.connect(server, 80)) // Connect to the server
{
    client.print(phant.post());
}
