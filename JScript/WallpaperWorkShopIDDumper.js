// ==UserScript==
// @name         Steam Wallpaper 创意工坊 id 提取
// @namespace    http://tampermonkey.net/
// @version      2024-08-03
// @description  try to take over the world!
// @author       You
// @match        https://steamcommunity.com/workshop/browse/*
// @icon         https://www.google.com/s2/favicons?sz=64&domain=steamcommunity.com
// @grant        none
// ==/UserScript==

(function () {
    'use strict';

    const page_max_get = 10;
    let do_dump = function () {
        const full = document.getElementsByClassName('workshopItem');
        let all = ''
        for (const element of full) {
            all = all.concat(element.getElementsByClassName('ugc')[0].getAttribute('data-publishedfileid')).concat('\n');
        }
        console.log(all)
        return all;
    }

    let do_next_page = function () {
        let current_location_search = window.location.search;
        let page_begin = current_location_search.indexOf('p=') + 2;
        let page_end = current_location_search.indexOf('&', current_location_search.indexOf('p=') + 2);
        let current_page_index = current_location_search.substring(page_begin, page_end == -1 ? current_location_search.length : page_end)

        let all_page_link = document.getElementsByClassName('pagelink');
        for (const element of all_page_link) {
            if (parseInt(element.text) === parseInt(current_page_index) + 1) {
                console.log(element.text)
                console.log(element)

                setTimeout(() => {
                    element.click();
                }, 1000)

            }
        }
    }


    if (localStorage.getItem('current_page') == null) {
        localStorage.setItem('current_page', '0')
    }
    if (localStorage.getItem('all_id') == null) {
        localStorage.setItem('all_id', '')
    }
    if (parseInt(localStorage.getItem('current_page')) < page_max_get) {
        localStorage.setItem('all_id', localStorage.getItem('all_id') + do_dump());
        localStorage.setItem('current_page', (parseInt(localStorage.getItem('current_page')) + 1).toString())
        do_next_page();
    }
})();