#include "Tilc/Utils/Archives.h"
//#include "Tilc/Utils/File.h"
#include <zlib.h>

int Tilc::TArchive::GzUncompress(const char* GzFilePath, Tilc::TExtString& Output)
{
    gzFile inFileZ = gzopen(GzFilePath, "rb");
    if (inFileZ == NULL)
    {
        return -1;
    }
    unsigned char unzipBuffer[8192];
    unsigned int unzippedBytes;
    //std::vector<unsigned char> unzippedData;
    Output = "";
    while (true)
    {
        unzippedBytes = gzread(inFileZ, unzipBuffer, 8192);
        if (unzippedBytes > 0)
        {
            Output.append(unzipBuffer, unzipBuffer + unzippedBytes);
        }
        else
        {
            break;
        }
    }
    gzclose(inFileZ);
    return Output.size();
}

int Tilc::TArchive::GzUncompress(const TExtString& Input, TExtString& Output)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 15 + 16) != Z_OK)
    {
        return -1;
    }

    zs.next_in = (Bytef*)Input.data();
    zs.avail_in = Input.size();

    int ret;
    char outbuffer[32768];

    Output.clear();
    do
    {
        zs.next_out = (Bytef*)outbuffer;
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (Output.size() < zs.total_out)
        {
            Output.append(outbuffer, zs.total_out - Output.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        return -2;
    }

    return zs.total_out;
}

int Tilc::TArchive::GzCompress(const TExtString& Input, TExtString& Output)
{
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 9, Z_DEFAULT_STRATEGY) != Z_OK)
    {
        return -1;
    }

    zs.next_in = (Bytef*)Input.data();
    zs.avail_in = Input.size();

    int ret;
    char outbuffer[32768];
    Output.clear();

    do
    {
        zs.next_out = (Bytef*)outbuffer;
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (Output.size() < zs.total_out)
        {
            Output.append(outbuffer, zs.total_out - Output.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END)
    {
        return -2;
    }

    return zs.total_out;
}
