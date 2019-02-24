int gpf()
{
    asm("int $99");
    return 0;
}