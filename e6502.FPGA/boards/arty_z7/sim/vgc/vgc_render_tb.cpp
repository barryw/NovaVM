// Reusable VGC render testbench (Verilator). Drives the VGC via its CPU command
// interface, runs the real video timing, captures one active frame of vid_r/g/b
// to a PPM so we can SEE what the VGC actually scans out — before any bitstream.
// First use: reproduce the copper-bar border/active misalignment.
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
// copper_add: set reg `r` = `v` at beam (x,y).  P0/P1=x P2=y P3/P4=reg P5=value, CMD=COPPERADD($1B)
static void copper_add(int x,int y,int r,int v){
    cpu_write(0xA011,x&0xFF); cpu_write(0xA012,(x>>8)&0xFF);
    cpu_write(0xA013,y&0xFF);
    cpu_write(0xA014,r&0xFF); cpu_write(0xA015,0);
    cpu_write(0xA016,v&0xFF);
    cpu_write(0xA010,0x1B); settle(10);
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
    settle(120000);                       // let the reset scrubber clear the memories
    cpu_write(0xA000,3);                   // GRAPHICS mode 3 (gfx only)
    cpu_write(0xA010,0x1C); settle(10);    // COPPERCLR
    for(int y=0;y<200;y+=8){
        int c=((y/16)&7)+1;
        copper_add(0,y,1,c);               // bg (reg 1)
        copper_add(0,y,13,c);              // border (reg 13)
    }
    cpu_write(0xA011,0); cpu_write(0xA010,0x21); settle(10);  // COPPERUSE 0
    cpu_write(0xA010,0x1D); settle(10);    // COPPERENA
    // ---- capture: collect rows of active pixels across frames; keep the 2nd full frame ----
    std::vector<std::vector<uint32_t>> rows, frame; std::vector<uint32_t> cur;
    int pde=0, pvs=dut->vid_vsync, frames=0;
    for(long t=0;t<4L*900*560 && frames<2;t++){
        tick();
        int de=dut->vid_de, vs=dut->vid_vsync;
        if(de) cur.push_back((dut->vid_r<<8)|(dut->vid_g<<4)|dut->vid_b);
        if(pde&&!de){ if(!cur.empty()){rows.push_back(cur);cur.clear();} }
        if(!pvs&&vs){ frames++; if(frames==1){rows.clear();} else {frame=rows;} }
        pde=de; pvs=vs;
    }
    // ---- write PPM ----
    size_t W=0; for(auto&r:frame) if(r.size()>W) W=r.size();
    size_t H=frame.size();
    fprintf(stderr,"frame: %zu rows, max width %zu\n",H,W);
    FILE*f=fopen("vgc.ppm","wb"); fprintf(f,"P6\n%zu %zu\n255\n",W,H);
    for(auto&r:frame){
        for(size_t x=0;x<W;x++){
            uint32_t px = x<r.size()? r[x] : 0;
            uint8_t R=((px>>8)&0xF)*17,G=((px>>4)&0xF)*17,B=(px&0xF)*17;
            fputc(R,f);fputc(G,f);fputc(B,f);
        }
    }
    fclose(f);
    fprintf(stderr,"wrote vgc.ppm\n");
    return 0;
}
