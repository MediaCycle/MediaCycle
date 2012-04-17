// From http://octave.sourceforge.net/octave_embed.tar.gz
#ifndef OCTAVE_EMBED_H__
#define OCTAVE_EMBED_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void octave_init(int argc, char *argv[]);
extern int octave_call(const char *string);
extern void octave_end(void);

#ifdef __cplusplus
}
#endif

#endif

