const pause = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

async function waitForElement(rootElement, elementClassName, maxTimeout = 3000) {
    const startTime = Date.now();
    while (Date.now() - startTime < maxTimeout) {
        const element = rootElement.getElementsByClassName(elementClassName);
        if (element !== null && element.length > 0) {
            return element;
        }
        await pause(50);  // 等待100毫秒再检查
    }
    throw new Error(`等待元素 ${elementClassName} 超时`);
}

async function getProjectNames(rootClass) {
    const titleClass = '.d-inline-block.mb-1';
    const btnClass = '.btn-sm.btn.BtnGroup-item.px-2.float-none';
    const itemClass = 'form-checkbox mt-1 mb-0 p-1';
    const checkedClass = '.mx-0.js-user-list-menu-item';
    const itemListNameClass = '.Truncate-text';


    async function logProjectStars(root) {
        const projectName = root.querySelector(titleClass).textContent.replace(/\n/g, '').replace(/\t/g, '').replace(/ {2,}/g, '');
        root.querySelector(btnClass).click();

        try {
            await waitForElement(root, itemClass);

            const starredLists = Array.from(root.getElementsByClassName(itemClass))
                .filter(item => item.querySelector(checkedClass).hasAttribute('checked'))
                .map(item => item.querySelector(itemListNameClass).textContent);

            starredLists.forEach(starListName => {
                console.log(`${projectName} has star in list: ${starListName}`);
            });

            if (starredLists.length === 0) {
                console.log(`${projectName} has no star in list !!!!!!`);
            }
        } catch (error) {
            console.error('wait for element error or timeout:', error);
        }
    }

    for (const root of document.getElementsByClassName(rootClass)) {
        await logProjectStars(root);
    }
}

function mainGotoNextPage(containerElementClassName, btnElementClassName) {
    const containerElement = document.getElementsByClassName(containerElementClassName)[0];
    const nextPageElement = containerElement.getElementsByClassName(btnElementClassName)[1];
    if (nextPageElement.hasAttribute('disabled')) {
        return false;
    }
    nextPageElement.click();
    return true;
}

async function main() {
    const main_containerClassName = 'paginate-container';
    const main_btnClassName = 'btn';
    const main_singleProjectClassName = 'col-12 d-block width-full py-4 border-bottom color-border-muted';

    while (true) {
        try {
            await getProjectNames(main_singleProjectClassName);
            if (!mainGotoNextPage(main_containerClassName, main_btnClassName)) {
                console.log('没有下一页了');
                break;
            }

            await pause(2000);
            await waitForElement(document, main_singleProjectClassName)
        } catch (error) {
            console.error('Error in main loop:', error);
            break;
        }
    }
}


/**
 * just copy to console and enter, show the which list collect this project
 */
main().then(() => console.log('Scraping completed')).catch(error => console.error('Fatal error:', error));