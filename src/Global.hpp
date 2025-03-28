#pragma once

#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "diffbuild.hpp"
#include "inttypes.hpp"
#include "pbg/PbgArchive.hpp"
#include <windows.h>

namespace th08
{
namespace FileSystem
{
LPBYTE Decrypt(LPBYTE inData, i32 size, u8 xorValue, u8 xorValueInc, i32 chunkSize, i32 maxBytes);
LPBYTE TryDecryptFromTable(LPBYTE inData, LPINT unused, i32 size);
LPBYTE Encrypt(LPBYTE inData, i32 size, u8 xorValue, u8 xorValueInc, i32 chunkSize, i32 maxBytes);
LPBYTE OpenFile(LPSTR path, i32 *fileSize, BOOL isExternalResource);
BOOL CheckIfFileAlreadyExists(LPCSTR path);
int WriteDataToFile(LPCSTR path, LPVOID data, size_t size);
}; // namespace FileSystem

class GameErrorContext
{
  public:
    GameErrorContext();
    ~GameErrorContext();

    void ResetContext()
    {
        m_BufferEnd = m_Buffer;
        m_BufferEnd[0] = '\0';
    }

    void Flush()
    {
        if (m_BufferEnd != m_Buffer)
        {
            Log("---------------------------------------------------------- \n");

            if (m_ShowMessageBox)
            {
                MessageBoxA(NULL, m_Buffer, "", MB_ICONSTOP);
            }

            FileSystem::WriteDataToFile("./log.txt", m_Buffer, strlen(m_Buffer));
        }
    }

    const char *Log(const char *fmt, ...);
    const char *Fatal(const char *fmt, ...);

  private:
    char m_Buffer[0x2000];
    char *m_BufferEnd;
    i8 m_ShowMessageBox;
};

class Rng
{
  public:
    u16 GetRandomU16();
    u32 GetRandomU32();
    f32 GetRandomF32();
    f32 GetRandomF32Signed();

    void ResetGenerationCount()
    {
        m_GenerationCount = 0;
    }

    void SetSeed(u16 newSeed)
    {
        m_Seed = newSeed;
    }

    u16 GetSeed()
    {
        return m_Seed;
    }

    u16 GetRandomU16InRange(u16 range)
    {
        return range != 0 ? GetRandomU16() % range : 0;
    }

    u32 GetRandomU32InRange(u32 range)
    {
        return range != 0 ? GetRandomU32() % range : 0;
    }

    f32 GetRandomF32InRange(f32 range)
    {
        return GetRandomF32() * range;
    }

    f32 GetRandomF32SignedInRange(f32 range)
    {
        return GetRandomF32Signed() * range;
    }

  private:
    u16 m_Seed, m_SeedBackup;
    u32 m_GenerationCount;
};

class ZunMemory
{
  public:
    ZunMemory();
    ~ZunMemory();

    // NOTE: the default parameter for debugText is probably just __FILE__
    void *Alloc(size_t size, const char *debugText = "d:\\cygwin\\home\\zun\\prog\\th08\\global.h")
    {
        return malloc(size);
    }

    void Free(void *ptr)
    {
        free(ptr);
    }

    void *AddToRegistry(void *ptr, size_t size, char *name)
    {
#ifdef DEBUG
        m_bRegistryInUse = TRUE;
        for (i32 i = 0; i < ARRAY_SIZE_SIGNED(m_Registry); i++)
        {
            if (m_Registry[i] == NULL)
            {
                RegistryInfo *info = (RegistryInfo *)malloc(sizeof(*info));
                if (info != NULL)
                {
                    info->data = ptr;
                    info->size = size;
                    info->name = name;
                    m_Registry[i] = info;
                }
                break;
            }
        }
#endif
        return ptr;
    }

    void RemoveFromRegistry(VOID *ptr)
    {
#ifdef DEBUG
        for (i32 i = 0; i < ARRAY_SIZE_SIGNED(m_Registry); i++)
        {
            if (m_Registry[i] == ptr)
            {
                free(m_Registry[i]);
                m_Registry[i] = NULL;
                break;
            }
        }
#endif
    }

  private:
    struct RegistryInfo
    {
        void *data;
        size_t size;
        char *name;
    };

    RegistryInfo *m_Registry[0x1000];
    BOOL m_bRegistryInUse;
};

DIFFABLE_EXTERN(Rng, g_Rng);
DIFFABLE_EXTERN(GameErrorContext, g_GameErrorContext);
DIFFABLE_EXTERN(PbgArchive, g_PbgArchive);
DIFFABLE_EXTERN(ZunMemory, g_ZunMemory);
DIFFABLE_EXTERN(ControllerMapping, g_ControllerMapping);
}; // namespace th08
