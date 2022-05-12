// SPDX-License-Identifier: GPL-2.0-only
/*
 * SpanDSP - a series of DSP components for telephony
 *
 * echo.c - A line echo canceller.  This code is being developed
 *          against and partially complies with G168.
 *
 * Written by Steve Underwood <steveu@coppice.org>
 *         and David Rowe <david_at_rowetel_dot_com>
 *
 * Copyright (C) 2001, 2003 Steve Underwood, 2007 David Rowe
 *
 * Based on a bit from here, a bit from there, eye of toad, ear of
 * bat, 15 years of failed attempts by David and a few fried brain
 * cells.
 *
 * All rights reserved.
 */

/*! \file */

/* Implementation Notes
   David Rowe
   April 2007

   This code started life as Steve's NLMS algorithm with a tap
   rotation algorithm to handle divergence during double talk.  I
   added a Geigel Double Talk Detector (DTD) [2] and performed some
   G168 tests.  However I had trouble meeting the G168 requirements,
   especially for double talk - there were always cases where my DTD
   failed, for example where near end speech was under the 6dB
   threshold required for declaring double talk.

   So I tried a two path algorithm [1], which has so far given better
   results.  The original tap rotation/Geigel algorithm is available
   in SVN http://svn.rowetel.com/software/oslec/tags/before_16bit.
   It's probably possible to make it work if some one wants to put some
   serious work into it.

   At present no special treatment is provided for tones, which
   generally cause NLMS algorithms to diverge.  Initial runs of a
   subset of the G168 tests for tones (e.g ./echo_test 6) show the
   current algorithm is passing OK, which is kind of surprising.  The
   full set of tests needs to be performed to confirm this result.

   One other interesting change is that I have managed to get the NLMS
   code to work with 16 bit coefficients, rather than the original 32
   bit coefficents.  This reduces the MIPs and storage required.
   I evaulated the 16 bit port using g168_tests.sh and listening tests
   on 4 real-world samples.

   I also attempted the implementation of a block based NLMS update
   [2] but although this passes g168_tests.sh it didn't converge well
   on the real-world samples.  I have no idea why, perhaps a scaling
   problem.  The block based code is also available in SVN
   http://svn.rowetel.com/software/oslec/tags/before_16bit.  If this
   code can be debugged, it will lead to further reduction in MIPS, as
   the block update code maps nicely onto DSP instruction sets (it's a
   dot product) compared to the current sample-by-sample update.

   Steve also has some nice notes on echo cancellers in echo.h

   References:

   [1] Ochiai, Areseki, and Ogihara, "Echo Canceller with Two Echo
       Path Models", IEEE Transactions on communications, COM-25,
       No. 6, June
       1977.
       https://www.rowetel.com/images/echo/dual_path_paper.pdf

   [2] The classic, very useful paper that tells you how to
       actually build a real world echo canceller:
	 Messerschmitt, Hedberg, Cole, Haoui, Winship, "Digital Voice
	 Echo Canceller with a TMS320020,
	 https://www.rowetel.com/images/echo/spra129.pdf

   [3] I have written a series of blog posts on this work, here is
       Part 1: http://www.rowetel.com/blog/?p=18

   [4] The source code http://svn.rowetel.com/software/oslec/

   [5] A nice reference on LMS filters:
	 https://en.wikipedia.org/wiki/Least_mean_squares_filter

   Credits:

   Thanks to Steve Underwood, Jean-Marc Valin, and Ramakrishnan
   Muthukrishnan for their suggestions and email discussions.  Thanks
   also to those people who collected echo samples for me such as
   Mark, Pawel, and Pavel.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "echo.h"

#define MIN_TX_POWER_FOR_ADAPTION	64
#define MIN_RX_POWER_FOR_ADAPTION	64
#define DTD_HANGOVER			600	/* 600 samples, or 75ms     */
#define DC_LOG2BETA			3	/* log2() of DC filter Beta */

/* adapting coeffs using the traditional stochastic descent (N)LMS algorithm */

static inline void lms_adapt_bg(struct oslec_state *ec, int clean, int shift)
{
	int i;

	int offset1;
	int offset2;
	int factor;
	int exp;

	if (shift > 0)
		factor = clean << shift;
	else
		factor = clean >> -shift;

	/* Update the FIR taps */

	offset2 = ec->curr_pos;
	offset1 = ec->taps - offset2;

	for (i = ec->taps - 1; i >= offset1; i--) {
		exp = (ec->fir_state_bg.history[i - offset1] * factor);
		ec->fir_taps16[1][i] += (int16_t) ((exp + (1 << 14)) >> 15);
	}
	for (; i >= 0; i--) {
		exp = (ec->fir_state_bg.history[i + offset2] * factor);
		ec->fir_taps16[1][i] += (int16_t) ((exp + (1 << 14)) >> 15);
	}
}

static inline int top_bit(unsigned int bits)
{
	if (bits == 0)
		return -1;
	else
		return (int)fls((int32_t) bits) - 1;
}

struct oslec_state *oslec_create(int len, int adaption_mode)
{
	struct oslec_state *ec;
	int i;
	const int16_t *history;

	ec = kzalloc(sizeof(*ec), GFP_KERNEL);
	if (!ec)
		return NULL;

	ec->taps = len;
	ec->log2taps = top_bit(len);
	ec->curr_pos = ec->taps - 1;

	ec->fir_taps16[0] =
	    kcalloc(ec->taps, sizeof(int16_t), GFP_KERNEL);
	if (!ec->fir_taps16[0])
		goto error_oom_0;

	ec->fir_taps16[1] =
	    kcalloc(ec->taps, sizeof(int16_t), GFP_KERNEL);
	if (!ec->fir_taps16[1])
		goto error_oom_1;

	history = fir16_create(&ec->fir_state, ec->fir_taps16[0], ec->taps);
	if (!history)
		goto error_state;
	history = fir16_create(&ec->fir_state_bg, ec->fir_taps16[1], ec->taps);
	if (!history)
		goto error_state_bg;

	for (i = 0; i < 5; i++)
		ec->xvtx[i] = ec->yvtx[i] = ec->xvrx[i] = ec->yvrx[i] = 0;

	ec->cng_level = 1000;
	oslec_adaption_mode(ec, adaption_mode);

	ec->snapshot = kcalloc(ec->taps, sizeof(int16_t), GFP_KERNEL);
	if (!ec->snapshot)
		goto error_snap;

	ec->cond_met = 0;
	ec->pstates = 0;
	ec->ltxacc = ec->lrxacc = ec->lcleanacc = ec->lclean_bgacc = 0;
	ec->ltx = ec->lrx = ec->lclean = ec->lclean_bg = 0;
	ec->tx_1 = ec->tx_2 = ec->rx_1 = ec->rx_2 = 0;
	ec->lbgn = ec->lbgn_acc = 0;
	ec->lbgn_upper = 200;
	ec->lbgn_upper_acc = ec->lbgn_upper << 13;

	return ec;

error_snap:
	fir16_free(&ec->fir_state_bg);
error_state_bg:
	fir16_free(&ec->fir_state);
error_state:
	kfree(ec->fir_taps16[1]);
error_oom_1:
	kfree(ec->fir_taps16[0]);
error_oom_0:
	kfree(ec);
	return NULL;
}
EXPORT_SYMBOL_GPL(oslec_create);

void oslec_free(struct oslec_state *ec)
{
	int i;

	fir16_free(&ec->fir_state);
	fir16_free(&ec->fir_state_bg);
	for (i = 0; i < 2; i++)
		kfree(ec->fir_taps16[i]);
	kfree(ec->snapshot);
	kfree(ec);
}
EXPORT_SYMBOL_GPL(oslec_free);

void oslec_adaption_mode(struct oslec_state *ec, int adaption_mode)
{
	ec->adaption_mode = adaption_mode;
}
EXPORT_SYMBOL_GPL(oslec_adaption_mode);

void oslec_flush(struct oslec_state *ec)
{
	int i;

	ec->ltxacc = ec->lrxacc = ec->lcleanacc = ec->lclean_bgacc = 0;
	ec->ltx = ec->lrx = ec->lclean = ec->lclean_bg = 0;
	ec->tx_1 = ec->tx_2 = ec->rx_1 = ec->rx_2 = 0;

	ec->lbgn = ec->lbgn_acc = 0;
	ec->lbgn_upper = 200;
	ec->lbgn_upper_acc = ec->lbgn_upper << 13;

	ec->nonupdate_dwell = 0;

	fir16_flush(&ec->fir_state);
	fir16_flush(&ec->fir_state_bg);
	ec->fir_state.curr_pos = ec->taps - 1;
	ec->fir_state_bg.curr_pos = ec->taps - 1;
	for (i = 0; i < 2; i++)
		memset(ec->fir_taps16[i], 0, ec->taps * sizeof(int16_t));

	ec->curr_pos = ec->taps - 1;
	ec->pstates = 0;
}
EXPORT_SYMBOL_GPL(oslec_flush);

void oslec_snapshot(struct oslec_state *ec)
{
	memcpy(ec->snapshot, ec->fir_taps16[0], ec->taps * sizeof(int16_t));
}
EXPORT_SYMBOL_GPL(oslec_snapshot);

/* Dual Path Echo Canceller */

int16_t oslec_update(struct oslec_state *ec, int16_t tx, int16_t rx)
{
	int32_t echo_value;
	int clean_bg;
	int tmp;
	int tmp1;

	/*
	 * Input scaling was found be required to prevent problems when tx
	 * starts clipping.  Another possible way to handle this would be the
	 * filter coefficent scaling.
	 */

	ec->tx = tx;
	ec->rx = rx;
	tx >>= 1;
	rx >>= 1;

	/*
	 * Filter DC, 3dB point is 160Hz (I think), note 32 bit precision
	 * required otherwise values do not track down to 0. Zero at DC, Pole
	 * at (1-Beta) on real axis.  Some chip sets (like Si labs) don't
	 * need this, but something like a $10 X100P card does.  Any DC really
	 * slows down convergence.
	 *
	 * Note: removes some low frequency from the signal, this reduces the
	 * speech quality when listening to samples through headphones but may
	 * not be obvious through a telephone handset.
	 *
	 * Note that the 3dB frequency in radians is approx Beta, e.g. for Beta
	 * = 2^(-3) = 0.125, 3dB freq is 0.125 rads = 159Hz.
	 */

	if (ec->adaption_mode & ECHO_CAN_USE_RX_HPF) {
		tmp = rx << 15;

		/*
		 * Make sure the gain of the HPF is 1.0. This can still
		 * saturate a little under impulse conditions, and it might
		 * roll to 32768 and need clipping on sustained peak level
		 * signals. However, the scale of such clipping is small, and
		 * the error due to any saturation should not markedly affect
		 * the downstream processing.
		 */
		tmp -= (tmp >> 4);

		ec->rx_1 += -(ec->rx_1 >> DC_LOG2BETA) + tmp - ec->rx_2;

		/*
		 * hard limit filter to prevent clipping.  Note that at this
		 * stage rx should be limited to +/- 16383 due to right shift
		 * above
		 */
		tmp1 = ec->rx_1 >> 15;
		if (tmp1 > 16383)
			tmp1 = 16383;
		if (tmp1 < -16383)
			tmp1 = -16383;
		rx = tmp1;
		ec->rx_2 = tmp;
	}

	/* Block average of power in the filter states.  Used for
	   adaption power calculation. */

	{
		int new, old;

		/* efficient "out with the old and in with the new" algorithm so
		   we don't have to recalculate over the whole block of
		   samples. */
		new = (int)tx * (int)tx;
		old = (int)ec->fir_state.history[ec->fir_state.curr_pos] *
		    (int)ec->fir_state.history[ec->fir_state.curr_pos];
		ec->pstates +=
		    ((new - old) + (1 << (ec->log2taps - 1))) >> ec->log2taps;
		if (ec->pstates < 0)
			ec->pstates = 0;
	}

	/* Calculate short term average levels using simple single pole IIRs */

	ec->ltxacc += abs(tx) - ec->ltx;
	ec->ltx = (ec->ltxacc + (1 << 4)) >> 5;
	ec->lrxacc += abs(rx) - ec->lrx;
	ec->lrx = (ec->lrxacc + (1 << 4)) >> 5;

	/* Foreground filter */

	ec->fir_state.coeffs = ec->fir_taps16[0];
	echo_value = fir16(&ec->fir_state, tx);
	ec->clean = rx - echo_value;
	ec->lcleanacc += abs(ec->clean) - ec->lclean;
	ec->lclean = (ec->lcleanacc + (1 << 4)) >> 5;

	/* Background filter */

	echo_value = fir16(&ec->fir_state_bg, tx);
	clean_bg = rx - echo_value;
	ec->lclean_bgacc += abs(clean_bg) - ec->lclean_bg;
	ec->lclean_bg = (ec->lclean_bgacc + (1 << 4)) >> 5;

	/* Background Filter adaption */

	/* Almost always adap bg filter, just simple DT and energy
	   detection to minimise adaption in cases of strong double talk.
	   However this is not critical for the dual path algorithm.
	 */
	ec->factor = 0;
	ec->shift = 0;
	if (!ec->nonupdate_dwell) {
		int p, logp, shift;

		/* Determine:

		   f = Beta * clean_bg_rx/P ------ (1)

		   where P is the total power in the filter states.

		   The Boffins have shown that if we obey (1) we converge
		   quickly and avoid instability.

		   The correct factor f must be in Q30, as this is the fixed
		   point format required by the lms_adapt_bg() function,
		   therefore the scaled version of (1) is:

		   (2^30) * f  = (2^30) * Beta * clean_bg_rx/P
		   factor      = (2^30) * Beta * clean_bg_rx/P     ----- (2)

		   We have chosen Beta = 0.25 by experiment, so:

		   factor      = (2^30) * (2^-2) * clean_bg_rx/P

		   (30 - 2 - log2(P))
		   factor      = clean_bg_rx 2                     ----- (3)

		   To avoid a divide we approximate log2(P) as top_bit(P),
		   which returns the position of the highest non-zero bit in
		   P.  This approximation introduces an error as large as a
		   factor of 2, but the algorithm seems to handle it OK.

		   Come to think of it a divide may not be a big deal on a
		   modern DSP, so its probably worth checking out the cycles
		   for a divide versus a top_bit() implementation.
		 */

		p = MIN_TX_POWER_FOR_ADAPTION + ec->pstates;
		logp = top_bit(p) + ec->log2taps;
		shift = 30 - 2 - logp;
		ec->shift = shift;

		lms_adapt_bg(ec, clean_bg, shift);
	}

	/* very simple DTD to make sure we dont try and adapt with strong
	   near end speech */

	ec->adapt = 0;
	if ((ec->lrx > MIN_RX_POWER_FOR_ADAPTION) && (ec->lrx > ec->ltx))
		ec->nonupdate_dwell = DTD_HANGOVER;
	if (ec->nonupdate_dwell)
		ec->nonupdate_dwell--;

	/* Transfer logic */

	/* These conditions are from the dual path paper [1], I messed with
	   them a bit to improve performance. */

	if ((ec->adaption_mode & ECHO_CAN_USE_ADAPTION) &&
	    (ec->nonupdate_dwell == 0) &&
	    /* (ec->Lclean_bg < 0.875*ec->Lclean) */
	    (8 * ec->lclean_bg < 7 * ec->lclean) &&
	    /* (ec->Lclean_bg < 0.125*ec->Ltx) */
	    (8 * ec->lclean_bg < ec->ltx)) {
		if (ec->cond_met == 6) {
			/*
			 * BG filter has had better results for 6 consecutive
			 * samples
			 */
			ec->adapt = 1;
			memcpy(ec->fir_taps16[0], ec->fir_taps16[1],
			       ec->taps * sizeof(int16_t));
		} else
			ec->cond_met++;
	} else
		ec->cond_met = 0;

	/* Non-Linear Processing */

	ec->clean_nlp = ec->clean;
	if (ec->adaption_mode & ECHO_CAN_USE_NLP) {
		/*
		 * Non-linear processor - a fancy way to say "zap small
		 * signals, to avoid residual echo due to (uLaw/ALaw)
		 * non-linearity in the channel.".
		 */

		if ((16 * ec->lclean < ec->ltx)) {
			/*
			 * Our e/c has improved echo by at least 24 dB (each
			 * factor of 2 is 6dB, so 2*2*2*2=16 is the same as
			 * 6+6+6+6=24dB)
			 */
			if (ec->adaption_mode & ECHO_CAN_USE_CNG) {
				ec->cng_level = ec->lbgn;

				/*
				 * Very elementary comfort noise generation.
				 * Just random numbers rolled off very vaguely
				 * Hoth-like.  DR: This noise doesn't sound
				 * quite right to me - I suspect there are some
				 * overflow issues in the filtering as it's too
				 * "crackly".
				 * TODO: debug this, maybe just play noise at
				 * high level or look at spectrum.
				 */

				ec->cng_rndnum =
				    1664525U * ec->cng_rndnum + 1013904223U;
				ec->cng_filter =
				    ((ec->cng_rndnum & 0xFFFF) - 32768 +
				     5 * ec->cng_filter) >> 3;
				ec->clean_nlp =
				    (ec->cng_filter * ec->cng_level * 8) >> 14;

			} else if (ec->adaption_mode & ECHO_CAN_USE_CLIP) {
				/* This sounds much better than CNG */
				if (ec->clean_nlp > ec->lbgn)
					ec->clean_nlp = ec->lbgn;
				if (ec->clean_nlp < -ec->lbgn)
					ec->clean_nlp = -ec->lbgn;
			} else {
				/*
				 * just mute the residual, doesn't sound very
				 * good, used mainly in G168 tests
				 */
				ec->clean_nlp = 0;
			}
		} else {
			/*
			 * Background noise estimator.  I tried a few
			 * algorithms here without much luck.  This very simple
			 * one seems to work best, we just average the level
			 * using a slow (1 sec time const) filter if the
			 * current level is less than a (experimentally
			 * derived) constant.  This means we dont include high
			 * level signals like near end speech.  When combined
			 * with CNG or especially CLIP seems to work OK.
			 */
			if (ec->lclean < 40) {
				ec->lbgn_acc += abs(ec->clean) - ec->lbgn;
				ec->lbgn = (ec->lbgn_acc + (1 << 11)) >> 12;
			}
		}
	}

	/* Roll around the taps buffer */
	if (ec->curr_pos <= 0)
		ec->curr_pos = ec->taps;
	ec->curr_pos--;

	if (ec->adaption_mode & ECHO_CAN_DISABLE)
		ec->clean_nlp = rx;

	/* Output scaled back up again to match input scaling */

	return (int16_t) ec->clean_nlp << 1;
}
EXPORT_SYMBOL_GPL(oslec_update);

/* This function is separated from the echo canceller is it is usually called
   as part of the tx process.  See rx HP (DC blocking) filter above, it's
   the same design.

   Some soft phones send speech signals with a lot of low frequency
   energy, e.g. down to 20Hz.  This can make the hybrid non-linear
   which causes the echo canceller to fall over.  This filter can help
   by removing any low frequency before it gets to the tx port of the
   hybrid.

   It can also help by removing and DC in the tx signal.  DC is bad
   for LMS algorithms.

   This is one of the classic DC removal filters, adjusted to provide
   sufficient bass rolloff to meet the above requirement to protect hybrids
   from things that upset them. The difference between successive samples
   produces a lousy HPF, and then a suitably placed pole flattens things out.
   The final result is a nicely rolled off bass end. The filtering is
   implemented with extended fractional precision, which noise shapes things,
   giving very clean DC removal.
*/

int16_t oslec_hpf_tx(struct oslec_state *ec, int16_t tx)
{
	int tmp;
	int tmp1;

	if (ec->adaption_mode & ECHO_CAN_USE_TX_HPF) {
		tmp = tx << 15;

		/*
		 * Make sure the gain of the HPF is 1.0. The first can still
		 * saturate a little under impulse conditions, and it might
		 * roll to 32768 and need clipping on sustained peak level
		 * signals. However, the scale of such clipping is small, and
		 * the error due to any saturation should not markedly affect
		 * the downstream processing.
		 */
		tmp -= (tmp >> 4);

		ec->tx_1 += -(ec->tx_1 >> DC_LOG2BETA) + tmp - ec->tx_2;
		tmp1 = ec->tx_1 >> 15;
		if (tmp1 > 32767)
			tmp1 = 32767;
		if (tmp1 < -32767)
			tmp1 = -32767;
		tx = tmp1;
		ec->tx_2 = tmp;
	}

	return tx;
}
EXPORT_SYMBOL_GPL(oslec_hpf_tx);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Rowe");
MODULE_DESCRIPTION("Open Source Line Echo Canceller");
MODULE_VERSION("0.3.0");
