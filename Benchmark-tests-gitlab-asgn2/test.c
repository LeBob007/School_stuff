#include <sys/libkern.h>

/*
 * Add the thread to the queue specified by its priority, and set the
 * corresponding status bit.
 */
void
runq_add(struct runq *rq, struct thread *td, int flags)
{
	struct rqhead *rqh;
	int pri;
	pri = td->td_priority / RQ_PPQ;
	if(12<= pri<= 19){
		pri = (random() % (8)) + 12;
	}
	if(30<=pri<=55){
		//implement splatter
		//srandom(time(null)); - does not work atm
		pri = (random() % (26)) + 30;
	}
	if(56<= pri<= 63){
		pri = (random() % (8)) + 56;
	}
	td->td_rqindex = pri;
	runq_setbit(rq, pri);
	rqh = &rq->rq_queues[pri];
	CTR4(KTR_RUNQ, "runq_add: td=%p pri=%d %d rqh=%p",
	    td, td->td_priority, pri, rqh);
	if(12<= pri<= 19 || 30<=pri<=63){
		struct thread *curr_td;
		struct thread *temp_td;
		int queue_size = 0;
		TAILQ_FOREACH_SAFE(curr_td, rqh, td_runq, temp_td){
			queue_size ++;
		}
		int ins_pos = random() % queue_size;//position to insert thread
		int curr_pos = 0;
		TAILQ_FOREACH_SAFE(curr_td, rqh, td_runq, temp_td){
			if(ins_pos<=curr_pos){
				TAILQ_INSERT_BEFORE(curr_td, td, td_runq);
				//end_ins_bool = 0;
				curr_pos ++;
				return;
			}
		}
		//if(end_ins_bool){
		TAILQ_INSERT_TAIL(rqh, td, td_runq);
	//	}
	}else{			
		if (flags & SRQ_PREEMPTED) {
			TAILQ_INSERT_HEAD(rqh, td, td_runq);
		} else {
			TAILQ_INSERT_TAIL(rqh, td, td_runq);
		}
	}
}

