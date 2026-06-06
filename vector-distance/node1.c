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

int connectcosts1[4] = { 1,  0,  1, 999 };



struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */

extern void tolayer2(struct rtpkt packet);
void printdt1(struct distance_table *dtptr);

int min1(int a,int b)
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

void send1(int arr[])
{
  struct rtpkt sendpkt;
  sendpkt.sourceid=1;
  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min1(count,dt1.costs[i][j]);
    }
    sendpkt.mincost[i]=count;
  }
    printf("Node 1's current mincost: [%d, %d, %d, %d]\n",
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
  if(arr!=NULL)
  {
    printf("Distance table updated: yes\n");
  }
   printf("Packets sent to: 0,2\n\n");
}


void rtinit1() 
{

    for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      dt1.costs[i][j]=999;
    }
  }
   for(int i=0;i<4;i++)
  {
  dt1.costs[1][i]=0;
  }
  dt1.costs[0][0]=2;
  dt1.costs[2][2]=1;

  printf("At time t=%.3f, rinit1() called\n", clocktime);
  printdt1(&dt1);
  send1(NULL);
  printf("Distance table initialized: yes\n");


}

void rtupdate1( struct rtpkt *rcvdpkt)
{
  int sid=rcvdpkt->sourceid;
  int did=rcvdpkt->destid;
  int *mic=rcvdpkt->mincost;

  if(did!=1)
  {
    return;
  }

  int old_mincost[4];

  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min1(count,dt1.costs[i][j]);
    }
    old_mincost[i]=count;
  }

    for(int i=0;i<4;i++)
    {
      dt1.costs[i][sid]=min1(dt1.costs[i][sid],dt1.costs[sid][sid]+mic[i]);
    }
  
    
  printf("At time t=%.3f, rtupdate1() called, sender=%d\n", clocktime, sid);
  send1(old_mincost);
  printdt1(&dt1);

  
  return;
  
}


void printdt1(struct distance_table *dtptr)
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



void linkhandler1(int linkid, int newcost)
{
 for(int i=0;i<4;i++)
  {
    if(i==1)
    {
      continue;
    }
    dt1.costs[i][linkid]=dt1.costs[i][linkid]-dt1.costs[linkid][linkid]+newcost;
  }

  printf("At time t=%.3f, linkhandler1() called\n", clocktime);
  printf("Link 1-%d cost changed to %d\n", linkid, newcost);
  printf("Distance table changed: yes\n");
  send1(NULL);
  printdt1(&dt1);
}
  
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	


