
#include <sys/syslog.h>
int pages_queued_for_flush = 0;
int pages_moved_to_cache = 0;
static int vm_pagemode;
static int vm_benchmark_on;

SYSCTL_INT(_vm, OID_AUTO, pagemode,
	CTLFLAG_RWTUN, &vm_pagemode, 0,
	"pagemode");

SYSCTL_INT(_vm, OID_AUTO, benchmark_on,
	CTLFLAG_RWTUN, &vm_benchmark_on, 0,
	"benchmark_on");

int
vm_pageout_flush(vm_page_t *mc, int count, int flags, int mreq, int *prunlen,
    boolean_t *eio)
{
	vm_object_t object = mc[0]->object;
	int pageout_status[count];
	int numpagedout = 0;
	int i, runlen;

	VM_OBJECT_ASSERT_WLOCKED(object);

	/*
	 * Initiate I/O.  Mark the pages busy and verify that they're valid
	 * and read-only.
	 *
	 * We do not have to fixup the clean/dirty bits here... we can
	 * allow the pager to do it after the I/O completes.
	 *
	 * NOTE! mc[i]->dirty may be partial or fragmented due to an
	 * edge case with file fragments.
	 */
	for (i = 0; i < count; i++) {
		KASSERT(mc[i]->valid == VM_PAGE_BITS_ALL,
		    ("vm_pageout_flush: partially invalid page %p index %d/%d",
			mc[i], i, count));
		KASSERT((mc[i]->aflags & PGA_WRITEABLE) == 0,
		    ("vm_pageout_flush: writeable page %p", mc[i]));
		vm_page_sbusy(mc[i]);
	}
	vm_object_pip_add(object, count);

	vm_pager_put_pages(object, mc, count, flags, pageout_status);

	runlen = count - mreq;
	if (eio != NULL)
		*eio = FALSE;
	for (i = 0; i < count; i++) {
		vm_page_t mt = mc[i];
                pages_queued_for_flush++;
		KASSERT(pageout_status[i] == VM_PAGER_PEND ||
		    !pmap_page_is_write_mapped(mt),
		    ("vm_pageout_flush: page %p is not write protected", mt));
		switch (pageout_status[i]) {
		case VM_PAGER_OK:
			vm_page_lock(mt);
			if (vm_page_in_laundry(mt))
				vm_page_deactivate_noreuse(mt);
			vm_page_unlock(mt);
			/* FALLTHROUGH */
		case VM_PAGER_PEND:
			numpagedout++;
			break;
		case VM_PAGER_BAD:
			/*
			 * The page is outside the object's range.  We pretend
			 * that the page out worked and clean the page, so the
			 * changes will be lost if the page is reclaimed by
			 * the page daemon.
			 */
			vm_page_undirty(mt);
			vm_page_lock(mt);
			if (vm_page_in_laundry(mt))
				vm_page_deactivate_noreuse(mt);
			vm_page_unlock(mt);
			break;
		case VM_PAGER_ERROR:
		case VM_PAGER_FAIL:
			/*
			 * If the page couldn't be paged out, then reactivate
			 * it so that it doesn't clog the laundry and inactive
			 * queues.  (We will try paging it out again later).
			 */
			vm_page_lock(mt);
			vm_page_activate(mt);
			vm_page_unlock(mt);
			if (eio != NULL && i >= mreq && i - mreq < runlen)
				*eio = TRUE;
			break;
		case VM_PAGER_AGAIN:
			if (i >= mreq && i - mreq < runlen)
				runlen = i - mreq;
			break;
		}

		/*
		 * If the operation is still going, leave the page busy to
		 * block all other accesses. Also, leave the paging in
		 * progress indicator set so that we don't attempt an object
		 * collapse.
		 */
		if (pageout_status[i] != VM_PAGER_PEND) {
			vm_object_pip_wakeup(object);
			vm_page_sunbusy(mt);
		}
	}
	if (prunlen != NULL)
		*prunlen = runlen;
	return (numpagedout);
}

static bool
vm_pageout_scan(struct vm_domain *vmd, int pass)
{
	vm_page_t m, next;
	struct vm_pagequeue *pq;
	vm_object_t object;
	long min_scan;
	int act_delta, addl_page_shortage, deficit, inactq_shortage, maxscan;
	int page_shortage, scan_tick, scanned, starting_page_shortage;
	boolean_t queue_locked;

        if (vm_pagemode == 0)
        {
            //printf("FreeBSD clock algorithm");
        }
        else
        {
            //printf("FIFO algorithm");
        }

	/*
	 * If we need to reclaim memory ask kernel caches to return
	 * some.  We rate limit to avoid thrashing.
	 */
	if (vmd == &vm_dom[0] && pass > 0 &&
	    (time_uptime - lowmem_uptime) >= lowmem_period) {
		/*
		 * Decrease registered cache sizes.
		 */
		SDT_PROBE0(vm, , , vm__lowmem_scan);
		EVENTHANDLER_INVOKE(vm_lowmem, VM_LOW_PAGES);
		/*
		 * We do this explicitly after the caches have been
		 * drained above.
		 */
		uma_reclaim();
		lowmem_uptime = time_uptime;
	}

	/*
	 * The addl_page_shortage is the number of temporarily
	 * stuck pages in the inactive queue.  In other words, the
	 * number of pages from the inactive count that should be
	 * discounted in setting the target for the active queue scan.
	 */
	addl_page_shortage = 0;

	/*
	 * Calculate the number of pages that we want to free.  This number
	 * can be negative if many pages are freed between the wakeup call to
	 * the page daemon and this calculation.
	 */
	if (pass > 0) {
		deficit = atomic_readandclear_int(&vm_pageout_deficit);
		page_shortage = vm_paging_target() + deficit;
	} else
		page_shortage = deficit = 0;
	starting_page_shortage = page_shortage;

	/*
	 * Start scanning the inactive queue for pages that we can free.  The
	 * scan will stop when we reach the target or we have scanned the
	 * entire queue.  (Note that m->act_count is not used to make
	 * decisions for the inactive queue, only for the active queue.)
	 */
	pq = &vmd->vmd_pagequeues[PQ_INACTIVE];
        int pages_in_inactive_queue_2 = maxscan = pq->pq_cnt;
	vm_pagequeue_lock(pq);
	queue_locked = TRUE;
        int pages_in_inactive_queue = 0;
        int pages_moved_to_higher_queue = 0;
        int pages_scanned = 0;
	for (m = TAILQ_FIRST(&pq->pq_pl);
	     m != NULL && maxscan-- > 0 && page_shortage > 0;
	     m = next) {
                pages_in_inactive_queue++;
                pages_scanned++;
		vm_pagequeue_assert_locked(pq);
		KASSERT(queue_locked, ("unlocked inactive queue"));
		KASSERT(vm_page_inactive(m), ("Inactive queue %p", m));

		PCPU_INC(cnt.v_pdpages);
		next = TAILQ_NEXT(m, plinks.q);

		/*
		 * skip marker pages
		 */
		if (m->flags & PG_MARKER)
			continue;

		KASSERT((m->flags & PG_FICTITIOUS) == 0,
		    ("Fictitious page %p cannot be in inactive queue", m));
		KASSERT((m->oflags & VPO_UNMANAGED) == 0,
		    ("Unmanaged page %p cannot be in inactive queue", m));

		/*
		 * The page or object lock acquisitions fail if the
		 * page was removed from the queue or moved to a
		 * different position within the queue.  In either
		 * case, addl_page_shortage should not be incremented.
		 */
		if (!vm_pageout_page_lock(m, &next))
			goto unlock_page;
		else if (m->hold_count != 0) {
			/*
			 * Held pages are essentially stuck in the
			 * queue.  So, they ought to be discounted
			 * from the inactive count.  See the
			 * calculation of inactq_shortage before the
			 * loop over the active queue below.
			 */
			addl_page_shortage++;
			goto unlock_page;
		}
		object = m->object;
		if (!VM_OBJECT_TRYWLOCK(object)) {
			if (!vm_pageout_fallback_object_lock(m, &next))
				goto unlock_object;
			else if (m->hold_count != 0) {
				addl_page_shortage++;
				goto unlock_object;
			}
		}
		if (vm_page_busied(m)) {
			/*
			 * Don't mess with busy pages.  Leave them at
			 * the front of the queue.  Most likely, they
			 * are being paged out and will leave the
			 * queue shortly after the scan finishes.  So,
			 * they ought to be discounted from the
			 * inactive count.
			 */
			addl_page_shortage++;
unlock_object:
			VM_OBJECT_WUNLOCK(object);
unlock_page:
			vm_page_unlock(m);
			continue;
		}
		KASSERT(m->hold_count == 0, ("Held page %p", m));

		/*
		 * Dequeue the inactive page and unlock the inactive page
		 * queue, invalidating the 'next' pointer.  Dequeueing the
		 * page here avoids a later reacquisition (and release) of
		 * the inactive page queue lock when vm_page_activate(),
		 * vm_page_free(), or vm_page_launder() is called.  Use a
		 * marker to remember our place in the inactive queue.
		 */
		TAILQ_INSERT_AFTER(&pq->pq_pl, m, &vmd->vmd_marker, plinks.q);
		vm_page_dequeue_locked(m);
		vm_pagequeue_unlock(pq);
		queue_locked = FALSE;


		/*
		 * Invalid pages can be easily freed. They cannot be
		 * mapped, vm_page_free() asserts this.
		 */
         if (vm_pagemode == 0){
             if (m->valid == 0)
     			goto free_page;
         }
         else{
             /*
              * Don't care whether the page needs to be requeued just free it as
              * it comes in
              */
			  if (m->valid == 0)
			  	goto free_page;
              goto clean_page;
         }

		/*
		 * If the page has been referenced and the object is not dead,
		 * reactivate or requeue the page depending on whether the
		 * object is mapped.
		 */
		if ((m->aflags & PGA_REFERENCED) != 0) {
			vm_page_aflag_clear(m, PGA_REFERENCED);
			act_delta = 1;
		} else
			act_delta = 0;
		if (object->ref_count != 0) {
			act_delta += pmap_ts_referenced(m);
		} else {
			KASSERT(!pmap_page_is_mapped(m),
			    ("vm_pageout_scan: page %p is mapped", m));
		}
		if (act_delta != 0) {
			if (object->ref_count != 0) {
				PCPU_INC(cnt.v_reactivated);
				vm_page_activate(m);
                                pages_moved_to_higher_queue++;
				/*
				 * Increase the activation count if the page
				 * was referenced while in the inactive queue.
				 * This makes it less likely that the page will
				 * be returned prematurely to the inactive
				 * queue.
 				 */
				m->act_count += act_delta + ACT_ADVANCE;
				goto drop_page;
			} else if ((object->flags & OBJ_DEAD) == 0) {
				vm_pagequeue_lock(pq);
				queue_locked = TRUE;
				m->queue = PQ_INACTIVE;
				TAILQ_INSERT_TAIL(&pq->pq_pl, m, plinks.q);
				vm_pagequeue_cnt_inc(pq);
				goto drop_page;
			}
		}

		/*
		 * If the page appears to be clean at the machine-independent
		 * layer, then remove all of its mappings from the pmap in
		 * anticipation of freeing it.  If, however, any of the page's
		 * mappings allow write access, then the page may still be
		 * modified until the last of those mappings are removed.
		 */
clean_page:
		if (object->ref_count != 0) {
			vm_page_test_dirty(m);
			if (m->dirty == 0)
				pmap_remove_all(m);
		}

		/*
		 * Clean pages can be freed, but dirty pages must be sent back
		 * to the laundry, unless they belong to a dead object.
		 * Requeueing dirty pages from dead objects is pointless, as
		 * they are being paged out and freed by the thread that
		 * destroyed the object.
		 */
		if (m->dirty == 0) {
free_page:
			vm_page_free(m);
                        pages_moved_to_cache++;
			PCPU_INC(cnt.v_dfree);
			--page_shortage;
		} else if ((object->flags & OBJ_DEAD) == 0)
			vm_page_launder(m);
drop_page:
		vm_page_unlock(m);
		VM_OBJECT_WUNLOCK(object);
		if (!queue_locked) {
			vm_pagequeue_lock(pq);
			queue_locked = TRUE;
		}
		next = TAILQ_NEXT(&vmd->vmd_marker, plinks.q);
		TAILQ_REMOVE(&pq->pq_pl, &vmd->vmd_marker, plinks.q);
	}
	vm_pagequeue_unlock(pq);

	/*
	 * Wake up the laundry thread so that it can perform any needed
	 * laundering.  If we didn't meet our target, we're in shortfall and
	 * need to launder more aggressively.
	 */
	if (vm_laundry_request == VM_LAUNDRY_IDLE &&
	    starting_page_shortage > 0) {
		pq = &vm_dom[0].vmd_pagequeues[PQ_LAUNDRY];
		vm_pagequeue_lock(pq);
		if (page_shortage > 0) {
			vm_laundry_request = VM_LAUNDRY_SHORTFALL;
			PCPU_INC(cnt.v_pdshortfalls);
		} else if (vm_laundry_request != VM_LAUNDRY_SHORTFALL)
			vm_laundry_request = VM_LAUNDRY_BACKGROUND;
		wakeup(&vm_laundry_request);
		vm_pagequeue_unlock(pq);
	}

	/*
	 * Wakeup the swapout daemon if we didn't free the targeted number of
	 * pages.
	 */
	if (page_shortage > 0)
		vm_swapout_run();

	/*
	 * If the inactive queue scan fails repeatedly to meet its
	 * target, kill the largest process.
	 */
	vm_pageout_mightbe_oom(vmd, page_shortage, starting_page_shortage);

	/*
	 * Compute the number of pages we want to try to move from the
	 * active queue to either the inactive or laundry queue.
	 *
	 * When scanning active pages, we make clean pages count more heavily
	 * towards the page shortage than dirty pages.  This is because dirty
	 * pages must be laundered before they can be reused and thus have less
	 * utility when attempting to quickly alleviate a shortage.  However,
	 * this weighting also causes the scan to deactivate dirty pages more
	 * more aggressively, improving the effectiveness of clustering and
	 * ensuring that they can eventually be reused.
	 */
	inactq_shortage = vm_cnt.v_inactive_target - (vm_cnt.v_inactive_count +
	    vm_cnt.v_laundry_count / act_scan_laundry_weight) +
	    vm_paging_target() + deficit + addl_page_shortage;
	inactq_shortage *= act_scan_laundry_weight;

	pq = &vmd->vmd_pagequeues[PQ_ACTIVE];
	vm_pagequeue_lock(pq);
        int pages_in_active_queue_2 = maxscan = pq->pq_cnt;

	/*
	 * If we're just idle polling attempt to visit every
	 * active page within 'update_period' seconds.
	 */
	scan_tick = ticks;
	if (vm_pagemode == 0 && vm_pageout_update_period != 0) {
		min_scan = pq->pq_cnt;
		min_scan *= scan_tick - vmd->vmd_last_active_scan;
		min_scan /= hz * vm_pageout_update_period;
	} else
		min_scan = 0;
	if (min_scan > 0 || (inactq_shortage > 0 && maxscan > 0))
		vmd->vmd_last_active_scan = scan_tick;

	/*
	 * Scan the active queue for pages that can be deactivated.  Update
	 * the per-page activity counter and use it to identify deactivation
	 * candidates.  Held pages may be deactivated.
	 */

        int pages_in_active_queue = 0;
        int pages_moved_to_lower_queue = 0;
	for (m = TAILQ_FIRST(&pq->pq_pl), scanned = 0; m != NULL && (scanned <
	    min_scan || (inactq_shortage > 0 && scanned < maxscan)); m = next,
	    scanned++) {
                pages_in_active_queue++;
                pages_scanned++;
		KASSERT(m->queue == PQ_ACTIVE,
		    ("vm_pageout_scan: page %p isn't active", m));
		next = TAILQ_NEXT(m, plinks.q);
		if ((m->flags & PG_MARKER) != 0)
			continue;
		KASSERT((m->flags & PG_FICTITIOUS) == 0,
		    ("Fictitious page %p cannot be in active queue", m));
		KASSERT((m->oflags & VPO_UNMANAGED) == 0,
		    ("Unmanaged page %p cannot be in active queue", m));
		if (!vm_pageout_page_lock(m, &next)) {
			vm_page_unlock(m);
			continue;
		}

		/*
		 * The count for page daemon pages is updated after checking
		 * the page for eligibility.
		 */
		PCPU_INC(cnt.v_pdpages);

		/*
		 * Check to see "how much" the page has been used.
		 */
		if ((m->aflags & PGA_REFERENCED) != 0) {
			vm_page_aflag_clear(m, PGA_REFERENCED);
			act_delta = 1;
		} else
			act_delta = 0;

		/*
		 * Perform an unsynchronized object ref count check.  While
		 * the page lock ensures that the page is not reallocated to
		 * another object, in particular, one with unmanaged mappings
		 * that cannot support pmap_ts_referenced(), two races are,
		 * nonetheless, possible:
		 * 1) The count was transitioning to zero, but we saw a non-
		 *    zero value.  pmap_ts_referenced() will return zero
		 *    because the page is not mapped.
		 * 2) The count was transitioning to one, but we saw zero.
		 *    This race delays the detection of a new reference.  At
		 *    worst, we will deactivate and reactivate the page.
		 */
		if (m->object->ref_count != 0)
			act_delta += pmap_ts_referenced(m);

		/*
		 * Advance or decay the act_count based on recent usage.
		 */
		if (act_delta != 0) {
			m->act_count += ACT_ADVANCE + act_delta;
			if (m->act_count > ACT_MAX)
				m->act_count = ACT_MAX;
		} else
			m->act_count -= min(m->act_count, ACT_DECLINE);
        if(vm_pagemode == 0)
        {
    		/*
    		 * Move this page to the tail of the active, inactive or laundry
    		 * queue depending on usage.
    		 */
    		if (m->act_count == 0) {
    			/* Dequeue to avoid later lock recursion. */
    			vm_page_dequeue_locked(m);

    			/*
    			 * When not short for inactive pages, let dirty pages go
    			 * through the inactive queue before moving to the
    			 * laundry queues.  This gives them some extra time to
    			 * be reactivated, potentially avoiding an expensive
    			 * pageout.  During a page shortage, the inactive queue
    			 * is necessarily small, so we may move dirty pages
    			 * directly to the laundry queue.
    			 */
    			if (inactq_shortage <= 0) {
                                    pages_moved_to_lower_queue++;
    				vm_page_deactivate(m);
                            }
    			else {
    				/*
    				 * Calling vm_page_test_dirty() here would
    				 * require acquisition of the object's write
    				 * lock.  However, during a page shortage,
    				 * directing dirty pages into the laundry
    				 * queue is only an optimization and not a
    				 * requirement.  Therefore, we simply rely on
    				 * the opportunistic updates to the page's
    				 * dirty field by the pmap.
    				 */
    				if (m->dirty == 0) {
    					vm_page_deactivate(m);
    					inactq_shortage -=
    					    act_scan_laundry_weight;
    				} else {
    					vm_page_launder(m);
    					inactq_shortage--;
    				}
    			}
    		} else
    			vm_page_requeue_locked(m);
        }
        else
        {
            /*
             * Here we just deactivate every active page and puts them into the
             * inactive queue as it comes through this should effectively create
             *  a FIFO queue
             */
             pages_moved_to_lower_queue++;
             vm_page_dequeue_locked(m);
             vm_page_deactivate(m);
        }
		vm_page_unlock(m);
	}
	vm_pagequeue_unlock(pq);
        if (vm_benchmark_on != 0) {
            log(LOG_INFO,
                "benchmark pages: pages_in_inactive_queue: %d, pages_in_active_queue: %d, inactive_queue_scanned: %d active_queue_scanned: %d queues_scanned: %d moved_to_higher_queue: %d moved_to_lower_queue: %d moved_to_cache: %d queued_for_flush: %d in_inactive_queue_2: %d, in_active_queue_2: %d\n",
                vm_cnt.v_inactive_count, vm_cnt.v_active_count,
                pages_in_inactive_queue, pages_in_active_queue,
                pages_scanned,
                pages_moved_to_higher_queue, pages_moved_to_lower_queue,
                pages_moved_to_cache, pages_queued_for_flush,
                pages_in_inactive_queue_2,  pages_in_active_queue_2);
        }
        pages_queued_for_flush = 0;
        pages_moved_to_cache = 0;

	if (pass > 0)
		vm_swapout_run_idle();
	return (page_shortage <= 0);
}