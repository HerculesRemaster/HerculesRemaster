# Licensing outreach: Disney's Hercules (1997) remaster

Plan and email drafts for getting a written commercial licence. Written 2026-10-13.

Until a signed licence exists, nothing is distributed: no free level, no web build, no store page, no
paid pre-orders (`authorization-scope.md`). The outreach material shows the work; it never ships the game.

## 1. What we are asking for

Disney owns everything in the product: the HERCULES.EXE/DLL code, art, characters, music and voice. Eurocom, the
original developer, closed in 2012. Disney delisted the PC release from Steam and GOG in January 2026, so the
game currently has no legal storefront.

There are two ways to get a deal, in order of likelihood:

| route | what we offer | who holds the Disney licence | money flow |
|---|---|---|---|
| **A. Technology partner** (most likely) | the runtime, 4K renderer, decoded formats, web/WASM port, plus the work to finish and port | an established publisher that already works with Disney (it signs with Disney, or Disney contracts it) | development fee (work-for-hire) and/or a revenue share; no minimum guarantee paid by us |
| **B. Direct licensee** | a finished remaster, self-published | us (needs a company, a lawyer, insurance and a minimum guarantee) | we pay Disney a minimum guarantee plus royalty and keep the rest |

Lead with A. Mention B only if Disney asks.

## 2. Where to send it

Disney says it does not accept unsolicited creative ideas. A third-party report says the permissions portal's
"game developer" category answers "We are not seeking unsolicited requests for licenses". Cold-pitching Disney
alone is therefore the weakest channel. Use these, in this order:

1. **The existing Disney contact** (if real and documented). `authorization-scope.md` records that the owner
   reports a development authorization from Disney. If that came from a named person, write to that person
   first (email 3.1), cite the date and reference, and ask them to route it to the games licensing team.
   Never describe the authorization more broadly than the written record.
2. **Publishers that already ship Disney retro titles.** These are the most realistic channel (email 3.2).
   - **Digital Eclipse** (owned by Atari since 2023, still takes outside projects) developed *Disney Classic
     Games Collection* and *The Disney Afternoon Collection*. Use the contact or business page on
     digitaleclipse.com.
   - **Nighthawk Interactive** published *Disney Classic Games Collection* (2021). Use the business contact on
     its site.
   - **Atari / Nightdive Studios** do retro remasters on their own engines. Use the business or partnership
     contact on atari.com and nightdivestudios.com.
   - **Limited Run Games** (Carbon Engine) ports retro titles for rights holders. Use the business contact on
     limitedrungames.com.
3. **Disney's permissions/licensing intake** (disneypermissions portal, "Mobile app or game developer"). It is
   low odds, but it creates a dated record. Replies can take up to 8 weeks.
4. **In person:** Licensing Expo (Las Vegas, May), GDC (San Francisco, March) and Gamescom (Cologne, August).
   Book meetings with the publishers above through the event's meeting system. A 2-minute demo on a laptop
   beats any email.
5. **LinkedIn:** look for business development or partnerships people at the publishers above. Send a short
   note (3.3) that links the video, with no attachments.

Always use addresses taken from the official sites at the time of sending. Do not guess personal emails.

## 3. Email drafts

Placeholders are in `[brackets]`. Keep the first email short. Do not attach binaries, extracted assets,
decompiled code or the RE documents. Link a private, unlisted video only.

### 3.1 To the existing Disney contact / Disney Games licensing

**Subject:** Disney's Hercules (1997) — 4K restoration ready for licensing review

> Dear [Name],
>
> [On [date] you / your team gave me permission to develop a 4K restoration of Disney's Hercules (1997)
> for non-commercial development (reference: [ref]).] I'm writing to share where it stands and to ask how
> to request a commercial licence, or who on the games licensing team I should speak with.
>
> What exists today, running on Windows 11:
> - The original 1997 game logic runs unchanged, so gameplay, timing and level design are exactly as shipped.
> - A new GPU renderer runs all ten levels in 4K at the original speed, with sharpened art that keeps
>   every character on-model.
> - A browser/WebAssembly version is in progress, which is the basis for Mac, Linux, mobile and console ports.
>
> The PC release left Steam and GOG in January 2026, and players still ask for it. I'd like to bring it back
> in a form Disney would be proud of, either with a publishing partner of Disney's choice or under a direct
> licence.
>
> A 2-minute private video: [unlisted link]
>
> I have not distributed any build or asset and will not do so without a written agreement. I'm happy to
> sign an NDA and show a live demo.
>
> Kind regards,
> Thanh Hoang-Minh
> [Company / title] · [phone] · [email] · [city, country]

If there is no prior contact, drop the bracketed first sentence and open with:
"I'm a game engineer who has rebuilt a 4K, multi-platform runtime for Disney's Hercules (1997)."

### 3.2 To a retro publisher (Digital Eclipse, Nighthawk, Atari/Nightdive, Limited Run)

**Subject:** Disney's Hercules (1997) — playable 4K runtime looking for a licence partner

> Hi [Name],
>
> Disney's Hercules (1997, Eurocom) was delisted from Steam and GOG in January 2026. It had a Very Positive
> rating there and no legal storefront since. I've built a working modern runtime for it and I'm looking for
> a partner that can secure the licence from Disney and publish it.
>
> What's done:
> - Runs the original game code unchanged (exact gameplay, timing and demos) with a new GPU renderer in
>   4K, across all 10 levels, at the original 30 Hz rate.
> - All formats are decoded: archive, sprites, levels, collision, palettes and sound.
> - An art pipeline with an exact fallback to the original pixels. HD art drops in per sprite or tile
>   without touching gameplay.
> - A WebAssembly port is in progress, which leads to Mac, Linux, Switch and PlayStation.
>
> What I'm proposing: you hold the licence and publish, and I deliver the game as a work-for-hire or
> revenue-share developer. Remaining work to a PC release is about [2–4] months. Console ports follow.
>
> Video (unlisted, 2 min): [link]
> One-page summary: [link to PDF]
>
> No build or asset has been or will be distributed without a licence. Happy to sign your NDA and give a
> live demo by call.
>
> Best,
> Thanh Hoang-Minh
> [Company / title] · [phone] · [email] · [portfolio / LinkedIn]

### 3.3 LinkedIn / short note (under 300 characters)

> Hi [Name] — I've built a 4K, multi-platform runtime for Disney's Hercules (1997), which was delisted from
> Steam in January 2026. I'm looking for a publisher who can take it to Disney. 2-min video: [link]. Open to a
> 15-minute call?

### 3.4 Follow-up (10 business days, then 4 weeks; no third follow-up)

**Subject:** Re: Disney's Hercules (1997) — 4K runtime

> Hi [Name], a quick follow-up on the Hercules restoration. Since my last note, [one concrete new thing:
> e.g. "the browser build now runs the first level"]. Video: [link]. If this isn't a fit, could you point me
> to the right person? Thanks — Thanh

## 4. What to prepare before sending

| item | content | effort |
|---|---|---|
| **Video, 2 min, unlisted** | 0:00 original 320×240 vs 4K side by side. 0:20 three different levels, with gameplay rather than menus. 0:50 HD art toggle. 1:10 browser build. 1:30 one slide: "runs original code, all 10 levels, ports ready". Use the Ophiotaur key art (`reference/CGI/Frozen Ophiotaur Boss Battle.png`) as the thumbnail, labelled "concept art". | 2–3 days |
| **One-pager (PDF)** | the problem (delisted, no storefront), what's built, platforms, timeline, the proposed deal (route A), contact | 1 day |
| **Live demo build** | runs from your laptop only, never handed over | done |
| **Company** | a registered company (LLC or the Vietnamese equivalent) to sign contracts and NDAs. Publishers rarely contract with individuals. | 1–4 weeks |
| **Lawyer** | an entertainment/IP lawyer for the NDA and term sheet. Pay by the hour; don't sign anything unreviewed. | when a reply arrives |

Do not say in writing that the game was "decompiled" or "cracked". Say "runs the original game code unchanged on
a new platform layer". That is accurate, and it is the selling point (exact fidelity).

## 5. Terms to aim for (route A)

| term | target | walk-away |
|---|---|---|
| development fee | paid monthly or per milestone to a PC release, then per port | none, or royalty-only with no advance |
| revenue share | 15–30 % of the publisher's net after Disney's royalty | |
| tech ownership | you keep the runtime/renderer IP and license it to the publisher for this title | assignment of all tech for no extra fee |
| credits | developer credit on the title | |
| scope | PC first, then Switch/PlayStation as paid add-ons | console ports included at no extra fee |
| art | upscaled original art (on-model) first; any redraw is approved by Disney and paid for | unpaid redraw work |

These figures are estimates, not market data. Confirm them with the lawyer.

## 6. Timeline and stop rule

| week | action |
|---|---|
| 1 | video, one-pager, company paperwork started |
| 2 | emails 3.1 and 3.2 to the existing Disney contact (if any) and 3–4 publishers; LinkedIn notes |
| 4 | first follow-up (3.4) |
| 8 | second and last follow-up; submit the Disney portal request if not done |
| 8–12 | calls, NDAs, demos with anyone who replied |

**Stop rule:** after 12 weeks with no NDA or term sheet from anyone, park the project and keep it as a portfolio
piece. Work on Tensormorph continues in parallel the whole time; outreach is about 2–4 weeks of effort spread
over 3 months.

## Sources

- Disney permissions intake: https://disneypermissions.my.salesforce-sites.com/WelcomeIntakePage
- Disney contact page (unsolicited ideas policy): https://thewaltdisneycompany.com/contact-us/
- Third-party report on the portal's "not seeking unsolicited requests" reply: https://www.forestrygames.com/p/disney-html5-games-licensing.html
- Disney licenses its IP to development partners rather than developing in-house: https://naavik.co/digest/disney-gaming-acquisitions/
- Delisting, January 2026: https://www.thegamer.com/disney-classic-games-delisted-steam/ , https://gamerant.com/disney-games-delisted-from-steam-list/
- Steam page (Very Positive, about 707 reviews): https://store.steampowered.com/app/987400/Disneys_Hercules/
- Digital Eclipse, Disney Classic Games: https://digitaleclipse.com/games/disneyclassicgames
- Nighthawk Interactive published the 2021 collection: https://vip-develop.primagames.com/?p=45803
- Atari acquires Digital Eclipse (still takes outside projects): https://atari.com/blogs/newsroom/atari-enters-into-an-agreement-to-acquire-digital-eclipse
- Limited Run Games, Carbon Engine: https://limitedrungames.com/blogs/news/limited-run-games-unveils-33-new-physical-releases-at-lrg3-summer-showcase
