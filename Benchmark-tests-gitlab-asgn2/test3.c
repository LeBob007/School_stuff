/*-
 * Copyright (c) 2001 Jake Burkholder <jake@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
	td->td_rqindex = pri;
	runq_setbit(rq, pri);
	rqh = &rq->rq_queues[pri];
	CTR4(KTR_RUNQ, "runq_add: td=%p pri=%d %d rqh=%p",
	    td, td->td_priority, pri, rqh);
	if(30<=pri<=63 || 12<=pri<=19){
		struct thread *curr_td;
		struct thread *temp_td;
		TAILQ_FOREACH_SAFE(curr_td, rqh, td_runq, temp_td){
			if(curr_td->td_priority > td -> td_priority){
				TAILQ_INSERT_BEFORE(curr_td, td, td_runq);
				return;
			}
		}
		TAILQ_INSERT_TAIL(rqh, td, td_runq);
	}else{			
		if (flags & SRQ_PREEMPTED) {
			TAILQ_INSERT_HEAD(rqh, td, td_runq);
		} else {
			TAILQ_INSERT_TAIL(rqh, td, td_runq);
		}
	}
}
