#ifndef ATA_H
#define ATA_H

void ata_init();

void ata_reset(int);
int ata_probe(int, int *, int *, char *);

int ata_read(int, void *, int, int);
int ata_write(int, void *, int, int);
int ata_read_bytes(int, void *, int, int);
int ata_write_bytes(int, void *, int nblocks, int offset);
int atapi_read(int, void *, int, int);

#endif
