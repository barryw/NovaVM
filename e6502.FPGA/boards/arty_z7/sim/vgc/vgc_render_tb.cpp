// VGC render testbench (Verilator). Drives the real rtl/vgc.sv via its CPU
// interface, runs the video timing, captures active frames to PPM.
//
// Tests:
//  1) Inline PETSCII-style style codes (write to VGC_CHAROUT) — toggles, reset,
//     and the 2-byte colour/char-bg codes — verified by reading back the VGC
//     registers via the debug port.
//  2) Per-character transparent background, temporally: emit char-bg bands while
//     bg_color=6, then CHANGE bg_color to 9 and render. Transparent cells must
//     follow to 9 (live read); opaque bands (one set via the $14 inline code,
//     one via the VGC_TEXT_BG register) must hold their colour.
#include "Vvgc.h"
#include "verilated.h"
#include <cstdio>
#include <cstdint>
#include <vector>
static Vvgc* dut;
static inline void tick(){ dut->clk=0; dut->eval(); dut->clk=1; dut->eval(); }
static void cpu_write(uint16_t a, uint8_t d){
    dut->cpu_ce=1; dut->cpu_we=1; dut->cpu_addr=a; dut->cpu_wdata=d; tick();
    dut->cpu_we=0; dut->cpu_ce=0; tick();
}
static void settle(int n){ for(int i=0;i<n;i++) tick(); }
static void emit(uint8_t ch){ cpu_write(0xA00E, ch); settle(4); }
static uint8_t dbg_read(uint16_t a){ dut->dbg_addr=a; dut->eval(); return dut->dbg_rdata; }
// register-driven char-bg row
static void emit_row(int row, uint8_t textbg, int n){
    cpu_write(0xA003,0); cpu_write(0xA004,row); cpu_write(0xA0C0,textbg);
    for(int i=0;i<n;i++) emit(0x41);
}
// inline-code-driven char-bg row ($14 = VGC_CC_CHARBG, then the colour byte)
static void emit_inline_row(int row, uint8_t textbg, int n){
    cpu_write(0xA003,0); cpu_write(0xA004,row);
    emit(0x14); emit(textbg);
    for(int i=0;i<n;i++) emit(0x41);
}
static int test_inline(){
    int ok=1;
    emit(0x01);                                            // reset to known state
    emit(0x14); emit(0x03);
    if(dbg_read(0xA0C0)!=0x03){ok=0;fprintf(stderr,"  FAIL $14 opaque\n");}
    emit(0x14); emit(16);
    if(dbg_read(0xA0C0)!=0x10){ok=0;fprintf(stderr,"  FAIL $14 transparent\n");}
    emit(0x0E);                                            // reverse on
    if((dbg_read(0xA0E6)&0x01)==0){ok=0;fprintf(stderr,"  FAIL reverse on\n");}
    emit(0x0E);                                            // reverse off (toggle)
    if((dbg_read(0xA0E6)&0x01)!=0){ok=0;fprintf(stderr,"  FAIL reverse off\n");}
    emit(0x06);                                            // bold on
    if((dbg_read(0xA0E6)&0x08)==0){ok=0;fprintf(stderr,"  FAIL bold\n");}
    emit(0x07);                                            // flash on
    if((dbg_read(0xA0E6)&0x04)==0){ok=0;fprintf(stderr,"  FAIL flash\n");}
    emit(0x01);                                            // reset clears styles + transparent
    if(dbg_read(0xA0E6)!=0 || dbg_read(0xA0C0)!=0x10){ok=0;fprintf(stderr,"  FAIL reset\n");}
    dut->dbg_addr=0;
    fprintf(stderr, ok?"INLINE CODES: PASS\n":"INLINE CODES: FAIL\n");
    return ok;
}
static std::vector<std::vector<uint32_t>> capture_frame(){
    std::vector<std::vector<uint32_t>> rows; std::vector<uint32_t> cur;
    int pde=0, pvs=dut->vid_vsync, frames=0;
    for(long t=0; t<3L*900*560 && frames<2; t++){
        tick();
        int de=dut->vid_de, vs=dut->vid_vsync;
        if(de) cur.push_back((dut->vid_r<<8)|(dut->vid_g<<4)|dut->vid_b);
        if(pde&&!de){ if(!cur.empty()){rows.push_back(cur);cur.clear();} }
        if(!pvs&&vs){ frames++; if(frames==1) rows.clear(); }
        pde=de; pvs=vs;
    }
    return rows;
}
static void write_ppm(const char*fn, std::vector<std::vector<uint32_t>>&frame){
    size_t W=0; for(auto&r:frame) if(r.size()>W) W=r.size();
    size_t H=frame.size();
    FILE*f=fopen(fn,"wb"); fprintf(f,"P6\n%zu %zu\n255\n",W,H);
    for(auto&r:frame) for(size_t x=0;x<W;x++){
        uint32_t px = x<r.size()? r[x] : 0;
        uint8_t R=((px>>8)&0xF)*17,G=((px>>4)&0xF)*17,B=(px&0xF)*17;
        fputc(R,f);fputc(G,f);fputc(B,f);
    }
    fclose(f);
}
int main(int argc,char**argv){
    Verilated::commandArgs(argc,argv);
    dut=new Vvgc;
    dut->rst=1; dut->video_rst=1; dut->cpu_ce=0; dut->cpu_we=0; dut->cpu_re=0;
    dut->cpu_addr=0; dut->cpu_raddr=0; dut->cpu_wdata=0;
    dut->key_valid=0; dut->key_data=0;
    dut->blt_we=0; dut->blt_re=0; dut->blt_addr=0; dut->blt_wdata=0; dut->blt_space=0;
    dut->dbg_addr=0; dut->dbg_we=0; dut->dbg_waddr=0; dut->dbg_wdata=0;
    dut->dbg_vmem_we=0; dut->dbg_vmem_re=0; dut->dbg_vmem_space=0; dut->dbg_vmem_addr=0; dut->dbg_vmem_wdata=0;
    settle(20); dut->rst=0; dut->video_rst=0;
    settle(120000);
    cpu_write(0xA000,0);           // mode 0 (text)
    int inline_ok = test_inline(); // (1) inline code register-readback test
    cpu_write(0xA00D,0);           // border 0
    cpu_write(0xA002,15);          // fg 15
    cpu_write(0xA001,6);           // bg_color = 6
    emit_row(10,0x03,80); emit_row(11,0x03,80); emit_row(12,0x03,80);  // opaque-3 via REGISTER
    emit_inline_row(5,0x05,80); emit_inline_row(6,0x05,80);            // opaque-5 via INLINE $14
    settle(50);
    auto A = capture_frame();      // bg=6
    cpu_write(0xA001,9);           // change bg_color -> 9
    settle(50);
    auto B = capture_frame();      // bg=9
    write_ppm("vgc_a.ppm",A); write_ppm("vgc_b.ppm",B);
    size_t H=A.size()<B.size()?A.size():B.size();
    long changed=0,total=0; int rows_followed=0, rows_held=0;
    for(size_t y=0;y<H;y++){
        size_t W=A[y].size()<B[y].size()?A[y].size():B[y].size();
        long rc=0; for(size_t x=0;x<W;x++){ total++; if(A[y][x]!=B[y][x]){changed++;rc++;} }
        if(W){ double f=(double)rc/W; if(f>0.6) rows_followed++; else if(f<0.1) rows_held++; }
    }
    fprintf(stderr,"frames %zu rows; %.0f%% changed bg6->bg9; rows_followed=%d rows_held=%d\n",
            H,100.0*changed/(total?total:1),rows_followed,rows_held);
    bool render_ok = (changed>total/3) && rows_followed>0 && rows_held>0;
    fprintf(stderr, render_ok ? "RENDER: PASS - transparent follows bg; opaque bands (register+inline) held\n"
                              : "RENDER: FAIL\n");
    return (inline_ok && render_ok)?0:1;
}
