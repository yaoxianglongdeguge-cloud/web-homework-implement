#include <stdio.h>


extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

struct distance_table 
{
  int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */

extern void tolayer2(struct rtpkt packet);
void printdt3(struct distance_table *dtptr);

int min3(int a,int b)
{
  if(a>=b)
  {
    return b;
  }
  else
  {
    return a;
  }
  return 0;
}

void send3(int arr[])
{
  struct rtpkt sendpkt;
  sendpkt.sourceid=3;
  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min3(count,dt3.costs[i][j]);
    }
    sendpkt.mincost[i]=count;
  }
    printf("Node 3's current mincost: [%d, %d, %d, %d]\n",
       sendpkt.mincost[0], sendpkt.mincost[1], sendpkt.mincost[2], sendpkt.mincost[3]);

   if(arr!=NULL)
  {
    int change=0;
    for(int m=0;m<4;m++)
    {
      if(arr[m]!=sendpkt.mincost[m])
      {
        change=1;
      }
    }
    if(change==0)
    {
      printf("Distance table updated: no\n");
      return;
    }
  }

  sendpkt.destid=0;
  tolayer2(sendpkt);
  sendpkt.destid=2;
  tolayer2(sendpkt);
  if(arr==NULL)
  {
    printf("Distance table initialized: yes\n");
  }
  else
  {
    printf("Distance table updated: yes\n");
  }
  printf("Packets sent to: 0,2\n\n");
}




void rtinit3() 
{
  
    for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      dt3.costs[i][j]=999;
    }
  }
  for(int i=0;i<4;i++)
  {
    dt3.costs[3][i]=0;
  }
  dt3.costs[0][0]=7;
  dt3.costs[2][2]=2;

  
  printf("At time t=%.3f, rinit3() called\n", clocktime);
  printdt3(&dt3);
  send3(NULL);

}




void rtupdate3(struct rtpkt *rcvdpkt)
{
  int sid=rcvdpkt->sourceid;
  int did=rcvdpkt->destid;
  int *mic=rcvdpkt->mincost;

  if(did!=3)
  {
    return;
  }

  int old_mincost[4];

  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min3(count,dt3.costs[i][j]);
    }
    old_mincost[i]=count;
  }

    for(int i=0;i<4;i++)
    {
      dt3.costs[i][sid]=min3(dt3.costs[i][sid],dt3.costs[sid][sid]+mic[i]);
    }
    

    
  printf("At time t=%.3f, rtupdate3() called, sender=%d\n", clocktime, sid);
  send3(old_mincost);
  printdt3(&dt3);
  
  return;
}


void printdt3(struct distance_table *dtptr)  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}







