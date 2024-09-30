

void InvalidInsCache()
{
    PlatInvalidInsCache();
}

void InvalidDataCache(unsigned long start, unsigned long end)
{
    PlatInvalidDateCache(start, end);
}

void CleanDataCache(unsigned long start, unsigned long end)
{
    PlatCleanDateCache(start, end);
}