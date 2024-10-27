// ==UserScript==
// @name         Remove Debugger
// @namespace    http://tampermonkey.net/
// @version      2024-08-13
// @description  try to take over the world!
// @author       You
// @run-at       document-start
// @match        *://www.17c216.com/*
// @icon         https://www.google.com/s2/favicons?sz=64&domain=aurorai.cn
// @grant        none
// ==/UserScript==

(function () {
    'use strict';

    function modifyElement(element) {
        if (element.tagName !== 'SCRIPT') {
            return;
        }
        if (element.innerHTML.length <= 0) {
            return;
        }
        // console.log(element.innerHTML)

        /// 修改这里
        if (element.innerHTML.includes('debuggerCheck();')) {
            console.log(element.innerHTML)
            // element.innerHTML = element.innerHTML.replace('debugger', '');
            element.innerHTML = ''
        }
    }

    // 使用 MutationObserver 监听 DOM 变化
    const observer = new MutationObserver((mutations) => {
        mutations.forEach((mutation) => {
            if (mutation.addedNodes && mutation.addedNodes.length > 0) {
                // 检查新加入的节点
                mutation.addedNodes.forEach((node) => {
                    if (node.nodeType === Node.ELEMENT_NODE) {
                        // 在这里修改新加入的元素
                        modifyElement(node);
                    }
                });
            }
        });
    });
    // 配置 observer
    observer.observe(document.documentElement, {
        childList: true,
        subtree: true
    });
})();