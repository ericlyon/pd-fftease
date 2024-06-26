/* FFTease for Pd */

#include "fftease.h"

static t_class *loopsea_class;

#define OBJECT_NAME "loopsea~"


typedef struct _loopsea
{
    t_object x_obj;
    t_float x_f;
    t_fftease *fft;
    t_float *frame_incr;
    t_float *store_incr;
    t_float *frame_phase;
    t_float *tbank; // bank of transposition factors
    t_float *interval_bank; // user provided intervals
    int interval_count; // how many intervals were provided
    t_float frameloc;
    t_float **loveboat;
    t_float current_frame;
    long *start_frames;
    long *end_frames;
    long framecount;
    long last_framecount;
    t_float frame_increment ;
    t_float fpos;
    t_float last_fpos;
    t_float tadv;
    int restart_loops_flag; // indicator to reset all loops from their start frames
    int tbank_flag; // indicator that we are using bin-level transposition factors
    int read_me;
    long frames_read;
    short mute;
    short playthrough;
    short lock;
    t_float duration;
    t_float sync;
    long interpolation_attr;
    void *listo; // list outlet
    t_atom *data; // storage for sending loop data to the outlet
} t_loopsea;

// t_float fftease_randf(t_float min, t_float max);
static void loopsea_dsp(t_loopsea *x, t_signal **sp);
static t_int *loopsea_perform(t_int *w);
static void *loopsea_new(t_symbol *msg, short argc, t_atom *argv);
static void loopsea_acquire_sample (t_loopsea *x) ;
static void loopsea_mute(t_loopsea *x, t_floatarg tog);
static void loopsea_setspeed( t_loopsea *x,  t_floatarg speed );
static void loopsea_free( t_loopsea *x );
static void loopsea_init(t_loopsea *x);
static void loopsea_randspeed(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv);
static void loopsea_playthrough(t_loopsea *x, t_floatarg state);
static void loopsea_transpose(t_loopsea *x, t_floatarg tf);
static void loopsea_synthresh(t_loopsea *x, t_floatarg thresh);
static void loopsea_oscbank(t_loopsea *x, t_floatarg flag);
static void loopsea_setloops(t_loopsea *x, t_floatarg minloop, t_floatarg maxloop);
static void loopsea_restart_loops(t_loopsea *x);
static void loopsea_randtransp(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv);
void loopsea_transp_choose(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv);
void loopsea_flat_transpose(t_loopsea *x);
void loopsea_readloops(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv);
void loopsea_printloops(t_loopsea *x);

void loopsea_tilde_setup(void)
{
    t_class *c;
    c = class_new(gensym("loopsea~"), (t_newmethod)loopsea_new,
                  (t_method)loopsea_free,sizeof(t_loopsea), 0,A_GIMME,0);
    CLASS_MAINSIGNALIN(c, t_loopsea, x_f);
    class_addmethod(c,(t_method)loopsea_dsp,gensym("dsp"), A_CANT, 0);
    class_addmethod(c,(t_method)loopsea_mute,gensym("mute"),A_FLOAT,0);
    class_addmethod(c,(t_method)loopsea_oscbank,gensym("oscbank"),A_FLOAT,0);
    class_addmethod(c,(t_method)loopsea_transpose,gensym("transpose"),A_FLOAT,0);
    class_addmethod(c,(t_method)loopsea_synthresh,gensym("synthresh"),A_FLOAT,0);
    class_addmethod(c,(t_method)loopsea_acquire_sample,gensym("acquire_sample"), 0);
    class_addmethod(c,(t_method)loopsea_randspeed, gensym("randspeed"), A_GIMME, 0);
    class_addmethod(c,(t_method)loopsea_randtransp, gensym("randtransp"), A_GIMME, 0);
    class_addmethod(c,(t_method)loopsea_transp_choose, gensym("transp_choose"), A_GIMME, 0);
    class_addmethod(c,(t_method)loopsea_setspeed, gensym("setspeed"),  A_FLOAT, 0);
    class_addmethod(c,(t_method)loopsea_playthrough, gensym("playthrough"),  A_DEFFLOAT, 0);
    class_addmethod(c,(t_method)loopsea_setloops, gensym("setloops"),  A_FLOAT, A_FLOAT, 0);
    class_addmethod(c,(t_method)loopsea_restart_loops, gensym("restart_loops"), 0);
    class_addmethod(c,(t_method)loopsea_flat_transpose, gensym("flat_transpose"),0);
    class_addmethod(c,(t_method)loopsea_printloops, gensym("printloops"),0);
    class_addmethod(c,(t_method)loopsea_readloops, gensym("readloops"),A_GIMME,0);
    loopsea_class = c;
    fftease_announce(OBJECT_NAME);
}

void loopsea_transpose(t_loopsea *x, t_floatarg tf)
{
    x->fft->P = tf;
}

void loopsea_synthresh(t_loopsea *x, t_floatarg thresh)
{
    x->fft->synt = thresh;
}

void loopsea_oscbank(t_loopsea *x, t_floatarg flag)
{
    x->fft->obank_flag = (short) flag;
}

void loopsea_store_incr(t_loopsea *x)
{
    t_fftease *fft = x->fft;

    int i;
    t_float *store_incr = x->store_incr;
    t_float *frame_incr = x->frame_incr;

    for(i = 0; i < fft->N2; i++){
        store_incr[i] = frame_incr[i];
    }
}

void loopsea_free(t_loopsea *x){
    int i ;

    if(x->fft->initialized){
        for(i = 0; i < x->framecount; i++){
            free(x->loveboat[i]) ;
        }
        free(x->loveboat);
        free(x->frame_phase);
        free(x->frame_incr);
        free(x->store_incr);
        free(x->start_frames);
        free(x->end_frames);
        free(x->tbank);
        free(x->interval_bank);
        free(x->data);
    }
    fftease_free(x->fft);
    free(x->fft);
}

void loopsea_setspeed( t_loopsea *x,  t_floatarg speed )
{
    t_fftease *fft = x->fft;
    if(! x->fft->init_status){
        return;
    }
    int i;
    for( i = 0; i < fft->N2; i++ ){
        x->frame_incr[i] = speed;
    }
}

void loopsea_setloops(t_loopsea *x, t_floatarg minloop, t_floatarg maxloop)
{
    long minframes, maxframes, loopframes, loopstart, loopend;
    int i;
    long framecount = x->framecount;
    long *start_frames = x->start_frames;
    long *end_frames = x->end_frames;
    t_fftease *fft = x->fft;
    t_float *frame_phase = x->frame_phase;

    // convert ms. to frames for min and max loop size

    minframes = (minloop/1000.0) / x->tadv;
    maxframes = (maxloop/1000.0) / x->tadv;

    if( (minframes < 0) || (minframes > framecount) ){
        minframes = 0;
    }
    if(maxframes > framecount){
        maxframes = framecount;
    }
    for( i = 0; i < fft->N2; i++ ){
        loopframes = (long) floor( fftease_randf((t_float)minframes, (t_float)maxframes) );
        loopstart = (long) floor( fftease_randf(0.0, (t_float)(framecount - loopframes)) );
        loopend = loopstart + loopframes;
        start_frames[i] = loopstart;
        end_frames[i] = loopend;
        frame_phase[i] = (t_float) loopstart;
    }
}


void loopsea_restart_loops(t_loopsea *x)
{
    x->restart_loops_flag = 1;
}

void loopsea_init(t_loopsea *x)
{
    int i;
    short initialized = x->fft->initialized;
    t_fftease  *fft = x->fft;
    fftease_init(fft);
    if(!fftease_msp_sanity_check(fft,OBJECT_NAME)){
        return;
    }

    x->current_frame = x->framecount = 0;
    x->fpos = x->last_fpos = 0;
    x->tadv = (t_float)fft->D/(t_float)fft->R;
    if(x->duration < 0.1){
        x->duration = 0.1;
    }
    x->framecount =  x->duration/x->tadv;
    x->read_me = 0;

    if(! initialized ){
        x->frame_increment = 1.0 ;
        x->mute = 0;
        x->playthrough = 0;
        x->sync = 0;
        x->frames_read = 0;
        x->frame_incr = (t_float *) calloc(fft->N2, sizeof(t_float));
        x->store_incr = (t_float *) calloc(fft->N2, sizeof(t_float));
        x->frame_phase = (t_float *) calloc(fft->N2, sizeof(t_float));
        x->tbank = (t_float *) calloc(fft->N2, sizeof(t_float));
        x->start_frames = (long *) calloc(fft->N2, sizeof(long));
        x->end_frames = (long *) calloc(fft->N2, sizeof(long));
        x->interval_bank = (t_float *) calloc(128, sizeof(t_float));
        x->loveboat = (t_float **) calloc(x->framecount, sizeof(t_float *));
        x->data = (t_atom *) calloc( ((fft->N2 * 3) + 1), sizeof(t_atom));
        for(i=0; i < x->framecount; i++){
            x->loveboat[i] = (t_float *) calloc((fft->N+2), sizeof(t_float));
            if(x->loveboat[i] == NULL){
                pd_error(0, "%s: Insufficient Memory!",OBJECT_NAME);
                return;
            }
        }
    }
    else {
        x->frame_incr = (t_float *) realloc(x->frame_incr, fft->N2 * sizeof(t_float));
        x->store_incr = (t_float *) realloc(x->store_incr, fft->N2 * sizeof(t_float));
        x->frame_phase = (t_float *) realloc(x->frame_phase, fft->N2 * sizeof(t_float));
        x->tbank = (t_float *) realloc(x->tbank, fft->N2 * sizeof(t_float));
        x->start_frames = (long *) realloc(x->start_frames, fft->N2 * sizeof(long));
        x->end_frames = (long *) realloc(x->end_frames, fft->N2 * sizeof(long));

        for(i = 0; i < x->last_framecount; i++){
            free(x->loveboat[i]) ;
        }
        x->loveboat = (t_float **)realloc(x->loveboat, x->framecount * sizeof(t_float*));
        for(i=0; i < x->framecount; i++){
            x->loveboat[i] = (t_float *) calloc((fft->N+2), sizeof(t_float));
            if(x->loveboat[i] == NULL){
                pd_error(0, "%s: Insufficient Memory!",OBJECT_NAME);
                return;
            }
        }
    }
    loopsea_setloops(x, 50.0, x->duration * 1000.0);
    x->last_framecount = x->framecount;
    // safety, probably not necessary:
    for(i = 0; i < fft->N2; i++){
        x->tbank[i] = 1.0;
    }
}

void *loopsea_new(t_symbol *msg, short argc, t_atom *argv)
{
t_fftease *fft;
    t_loopsea *x = (t_loopsea *)pd_new(loopsea_class);


    outlet_new(&x->x_obj, gensym("signal"));
    outlet_new(&x->x_obj, gensym("signal"));
    x->listo = outlet_new(&x->x_obj, gensym("list"));
    x->fft = (t_fftease *) calloc(1, sizeof(t_fftease) );
    fft = x->fft;
    fft->initialized = 0;

    srand(clock()); // needed ?
    // x->interpolation_attr = 0;
    x->restart_loops_flag = 0;
    fft->N = FFTEASE_DEFAULT_FFTSIZE;
    fft->overlap = FFTEASE_DEFAULT_OVERLAP;
    fft->winfac = FFTEASE_DEFAULT_WINFAC;
    if(argc > 0){ x->duration = atom_getfloatarg(0, argc, argv) / 1000.0; }
    else {
        post("%s: no duration given, using a default of 5000 ms",OBJECT_NAME);
        x->duration = 5.0;
    }
    if(argc > 1){ fft->N = (int) atom_getfloatarg(1, argc, argv); }
    if(argc > 2){ fft->overlap = (int) atom_getfloatarg(2, argc, argv); }
    return x;
}

static void do_loopsea(t_loopsea *x)
{
    t_fftease *fft = x->fft;

    int amp, freq, i;
    long minphase, maxphase, thisFramecount;
    int N = fft->N;
    int N2 = fft->N2;
    t_float fframe = x->current_frame ;
    t_float last_fpos = x->last_fpos ;
    int framecount = x->framecount;
    t_float *frame_incr = x->frame_incr;
    t_float *frame_phase = x->frame_phase;
    t_float *channel = fft->channel;
    long *start_frames = x->start_frames;
    long *end_frames = x->end_frames;
    int tbank_flag = x->tbank_flag;
    t_float *tbank = x->tbank;
    t_float frak;
    long iphase1, iphase2;

    if(x->read_me && x->framecount > 0){

        fftease_fold(fft);
        fftease_rdft(fft,FFT_FORWARD);
        fftease_convert(fft);
        for(i = 0; i < N; i++){
            x->loveboat[x->frames_read][i] = channel[i];
        }
        x->frames_read++;
        if(x->frames_read >= x->framecount){
            x->read_me = 0;
            // post("sample acquisition completed");
        }
        x->sync = (t_float) x->frames_read / (t_float) x->framecount;
    }
    else {
        for( i = 0 ; i < N2; i++ ){
            amp = i<<1;
            freq = amp + 1;
            minphase = start_frames[i];
            maxphase = end_frames[i];
            thisFramecount = maxphase - minphase;
            iphase1 = floor( frame_phase[i] );
            frak = frame_phase[i] - iphase1;
            if(iphase1 < minphase){
                iphase1 = minphase;
            }
            if( iphase1 > maxphase){
                iphase1 = maxphase;
            }
            //possible bug if increment is less than 0.0
            iphase2 = (iphase1 + 1) % framecount;

            // only interpolate if the fraction is greater than epsilon 0.0001
            if(frak < 0.0001 ){
                channel[amp] = x->loveboat[iphase1][amp];
                channel[freq] = x->loveboat[iphase1][freq];
            } else {
                channel[amp] = x->loveboat[iphase1][amp] + (frak *
                                                            (x->loveboat[iphase2][amp] - x->loveboat[iphase1][amp]));
                channel[freq] = x->loveboat[iphase1][freq] + (frak *
                                                              (x->loveboat[iphase2][freq] - x->loveboat[iphase1][freq]));
            }
            if(tbank_flag){
                channel[freq] *= tbank[i];
            }
            frame_phase[i] += frame_incr[i];
            while( frame_phase[i] > maxphase){
                frame_phase[i] -= thisFramecount;
            }
            while( frame_phase[i] < minphase ){
                frame_phase[i] += thisFramecount;
            }
        }
    }

    if(fft->obank_flag){
        fftease_oscbank(fft);
    } else {
        fftease_unconvert(fft);
        fftease_rdft(fft,FFT_INVERSE);
        fftease_overlapadd(fft);
    }

    /* restore state variables */

    x->current_frame = fframe;
    x->last_fpos = last_fpos;
}

t_int *loopsea_perform(t_int *w)
{
    int i, j;
    //////////////////////////////////////////////
    t_loopsea *x = (t_loopsea *) (w[1]);
    t_float *MSPInputVector = (t_float *)(w[2]);
    t_float *MSPOutputVector = (t_float *)(w[3]);
    t_float *sync_vec = (t_float *)(w[4]);

    /* dereference structure */

    t_fftease *fft = x->fft;
    int D = fft->D;
    int Nw = fft->Nw;
    t_float *input = fft->input;
    t_float *output = fft->output;
    t_float mult = fft->mult;
    int MSPVectorSize = fft->MSPVectorSize;
    t_float *internalInputVector = fft->internalInputVector;
    t_float *internalOutputVector = fft->internalOutputVector;
    int operationRepeat = fft->operationRepeat;
    int operationCount = fft->operationCount;


    if(x->mute){
        for(i=0; i < MSPVectorSize; i++){ MSPOutputVector[i] = 0.0; }
        for(i=0; i < MSPVectorSize; i++){ sync_vec[i] = 0.0; }
        return w+5;
    }
    if( fft->obank_flag ){
        mult *= FFTEASE_OSCBANK_SCALAR;
    }
    if(x->playthrough && x->read_me){
        for (i = 0; i < MSPVectorSize; i++) {
            MSPOutputVector[i] = MSPInputVector[i] * 0.5; // scale down
        }
        for(i=0; i < MSPVectorSize; i++){ sync_vec[i] = 0.0; }
        return w+5;
    }
    if(x->restart_loops_flag == 1){
        for( i = 0; i < x->fft->N2; i++ ){
            x->frame_phase[i] = (t_float) x->start_frames[i];
        }
        x->restart_loops_flag = 0;
    }


    if( fft->bufferStatus == EQUAL_TO_MSP_VECTOR ){
        memcpy(input, input + D, (Nw - D) * sizeof(t_float));
        memcpy(input + (Nw - D), MSPInputVector, D * sizeof(t_float));

        do_loopsea(x);

        for ( j = 0; j < D; j++ ){ *MSPOutputVector++ = output[j] * mult; }
        memcpy(output, output + D, (Nw-D) * sizeof(t_float));
        for(j = (Nw-D); j < Nw; j++){ output[j] = 0.0; }
    }
    else if( fft->bufferStatus == SMALLER_THAN_MSP_VECTOR ) {
        for( i = 0; i < operationRepeat; i++ ){
            memcpy(input, input + D, (Nw - D) * sizeof(t_float));
            memcpy(input + (Nw-D), MSPInputVector + (D*i), D * sizeof(t_float));

            do_loopsea(x);

            for ( j = 0; j < D; j++ ){ *MSPOutputVector++ = output[j] * mult; }
            memcpy(output, output + D, (Nw-D) * sizeof(t_float));
            for(j = (Nw-D); j < Nw; j++){ output[j] = 0.0; }
        }
    }
    else if( fft->bufferStatus == BIGGER_THAN_MSP_VECTOR ) {
        memcpy(internalInputVector + (operationCount * MSPVectorSize), MSPInputVector,MSPVectorSize * sizeof(t_float));
        memcpy(MSPOutputVector, internalOutputVector + (operationCount * MSPVectorSize),MSPVectorSize * sizeof(t_float));
        operationCount = (operationCount + 1) % operationRepeat;

        if( operationCount == 0 ) {
            memcpy(input, input + D, (Nw - D) * sizeof(t_float));
            memcpy(input + (Nw - D), internalInputVector, D * sizeof(t_float));

            do_loopsea(x);

            for ( j = 0; j < D; j++ ){ internalOutputVector[j] = output[j] * mult; }
            memcpy(output, output + D, (Nw - D) * sizeof(t_float));
            for(j = (Nw-D); j < Nw; j++){ output[j] = 0.0; }
        }
        fft->operationCount = operationCount;
    }
    for ( i = 0; i < MSPVectorSize; i++ ){
        sync_vec[i] = x->sync;
    }
    return w+5;
}

void loopsea_acquire_sample(t_loopsea *x)
{
    x->read_me = 1;
    x->frames_read = 0;
    return;
}

void loopsea_mute(t_loopsea *x, t_floatarg tog)
{
    x->mute = tog;
}

void loopsea_playthrough(t_loopsea *x, t_floatarg state)
{
    x->playthrough = state;
}

/*
void loopsea_linephase(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;
    int bin1, bin2;
    t_float phase1, phase2, bindiff;
    int i;
    t_float m1, m2;

    bin1 = (int) atom_getfloatarg(0, argc, argv);
    phase1 = atom_getfloatarg(1, argc, argv) * x->framecount;
    bin2 = (int) atom_getfloatarg(2, argc, argv);
    phase2 = atom_getfloatarg(3, argc, argv) * x->framecount;

    if( bin1 > fft->N2 || bin2 > fft->N2 ){
        pd_error(0, "too high bin number");
        return;
    }
    bindiff = bin2 - bin1;
    if( bindiff < 1 ){
        pd_error(0, "make bin2 higher than bin 1, bye now");
        return;
    }
    for( i = bin1; i < bin2; i++ ){
        m2 = (t_float) i / bindiff;
        m1 = 1. - m2;
        x->frame_phase[i] = m1 * phase1 + m2 * phase2;
    }
}
*/
void loopsea_randphase(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;

    t_float minphase, maxphase;
    int i;
    int framecount = x->framecount;

    minphase = atom_getfloatarg(0, argc, argv);
    maxphase = atom_getfloatarg(1, argc, argv);

    //  post("minphase %f maxphase %f",minphase, maxphase);
    if(minphase < 0.0)
        minphase = 0.0;
    if( maxphase > 1.0 )
        maxphase = 1.0;

    for( i = 0; i < fft->N2; i++ ){
        x->frame_phase[i] = (int) (fftease_randf( minphase, maxphase ) * (t_float) (framecount - 1) ) ;
    }
}

void loopsea_randspeed(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;

    t_float minspeed, maxspeed;
    int i;


    minspeed = atom_getfloatarg(0, argc, argv);
    maxspeed = atom_getfloatarg(1, argc, argv);

    for( i = 0; i < fft->N2; i++ ){
        x->frame_incr[i] = fftease_randf(minspeed, maxspeed);
    }
}



void loopsea_printloops(t_loopsea *x)
{
    t_fftease *fft = x->fft;
    t_atom *data = x->data;
    t_symbol *readloops = gensym("readloops");
    long *start_frames = x->start_frames;
    long *end_frames = x->end_frames;
    t_float *tbank = x->tbank;
    t_float *frame_incr = x->frame_incr;
    int data_index = 0;
    int i;
    SETSYMBOL(data+data_index, readloops);
    data_index++;
    for(i = 0; i < fft->N2; i++){
        SETFLOAT(data+data_index, (t_float)start_frames[i]);
        data_index++;
        SETFLOAT(data+data_index, (t_float)end_frames[i]);
        data_index++;
        SETFLOAT(data+data_index, tbank[i]);
        data_index++;
        SETFLOAT(data+data_index, frame_incr[i]);
        data_index++;
    }
    outlet_list(x->listo, 0, data_index, data);
}

void loopsea_readloops(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    int i,j;
    t_fftease *fft = x->fft;
    long *start_frames = x->start_frames;
    long *end_frames = x->end_frames;
    t_float *tbank = x->tbank;
    t_float *frame_incr = x->frame_incr;
    if(argc != fft->N2 * 4){
        post("readloops~: Houston we have a problem: mismatch between expected size %d, and actual message size %d",fft->N2*3, argc);
        return;
    }
    for(i = 0, j = 0; i < fft->N2; i++, j += 4){
        start_frames[i] = (long) atom_getfloatarg(j,argc,argv);
        end_frames[i] = (long) atom_getfloatarg(j+1,argc,argv);
        tbank[i] = atom_getfloatarg(j+2,argc,argv);
        frame_incr[i] = atom_getfloatarg(j+3,argc,argv);
    }
}

void loopsea_randtransp(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;

    t_float minincr, maxincr;
    int i;

    minincr = atom_getfloatarg(0, argc, argv);
    maxincr = atom_getfloatarg(1, argc, argv);
    for( i = 0; i < fft->N2; i++ ){
        x->tbank[i] = fftease_randf(minincr, maxincr);
    }
    x->tbank_flag = 1;
}

void loopsea_flat_transpose(t_loopsea *x)
{
    x->tbank_flag = 0;
}

void loopsea_transp_choose(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;

//    t_float minincr, maxincr;
    int i, randex;
    int interval_count = x->interval_count;
    t_float *interval_bank = x->interval_bank;
    if(argc < 2){
        pd_error(x, "transp_choose must receive at least two interval choices");
        return;
    }
    if(argc > 128){
        pd_error(x, "transp_choose allows a maximum of 128 intervals");
        return;
    }
    interval_count = argc;
    for(i = 0; i < argc; i++){
        interval_bank[i] = atom_getfloatarg(i, argc, argv);
    }
    for( i = 0; i < fft->N2; i++ ){
        randex = fftease_randi(0, interval_count - 1);
        x->tbank[i] = interval_bank[randex];
    }
    x->tbank_flag = 1;
}
/*
void loopsea_linespeed(t_loopsea *x, t_symbol *msg, short argc, t_atom *argv)
{
    t_fftease *fft = x->fft;
    int bin1, bin2;
    t_float speed1, speed2, bindiff;
    int i;
    t_float m1, m2;

    bin1 = (int) atom_getfloatarg(0, argc, argv);
    speed1 = atom_getfloatarg(1, argc, argv);
    bin2 = (int) atom_getfloatarg(2, argc, argv);
    speed2 = atom_getfloatarg(3, argc, argv);

    if( bin1 > fft->N2 || bin2 > fft->N2 ){
        pd_error(0, "too high bin number");
        return;
    }
    bindiff = bin2 - bin1;
    if( bindiff < 1 ){
        pd_error(0, "make bin2 higher than bin 1, bye now");
        return;
    }
    for( i = bin1; i < bin2; i++ ){
        m2 = (t_float) i / bindiff;
        m1 = 1. - m2;
        x->frame_incr[i] = m1 * speed1 + m2 * speed2;
    }
}
*/
void loopsea_dsp(t_loopsea *x, t_signal **sp)
{
    int reset_required = 0;
    int maxvectorsize = sp[0]->s_n;
    int samplerate = sp[0]->s_sr;

    if(!samplerate)
        return;
    t_fftease *fft = x->fft;
    if(fft->R != samplerate || fft->MSPVectorSize != maxvectorsize || fft->initialized == 0){
        reset_required = 1;
    }
    if(fft->MSPVectorSize != maxvectorsize){
        fft->MSPVectorSize = maxvectorsize;
        fftease_set_fft_buffers(fft);
    }
    if(fft->R != samplerate){
        fft->R = samplerate;
    }
    if(reset_required){
        loopsea_init(x);
    }
    if(fftease_msp_sanity_check(fft,OBJECT_NAME)) {
        dsp_add(loopsea_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
    }
}
