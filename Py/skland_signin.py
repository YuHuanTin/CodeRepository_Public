import hashlib
import hmac
import json
import os
import time
from urllib import parse

import requests


def checkResponseStatus(response_json):
    if response_json['status'] != 0:
        print('error!')
        exit(-1)
    return


def checkResponseCode(response_json):
    if response_json['code'] != 0:
        print('error!')
        exit(-1)
    return


def login(phone: str, password: str):
    '''
    获取 鹰角网络 登录 token
    :param phone: 
    :param password: 
    :return: token
    '''
    post_data = {
        'phone': phone,
        'password': password
    }

    result = requests.post('https://as.hypergryph.com/user/auth/v1/token_by_phone_password', data=post_data)
    json_result = result.json()

    checkResponseStatus(json_result)
    login_token = json_result['data']['token']
    return login_token


def get_cred(login_token: str):
    '''
    获取 cred 和 cred_token
    :param login_token: 
    :return: cred cred_token
    '''
    # 获取 OAuth2
    post_data = {
        'token': login_token,
        'appCode': '4ca99fa6b56cc2ba',
        'type': 0
    }
    result = requests.post('https://as.hypergryph.com/user/oauth2/v2/grant', data=post_data)
    json_result = result.json()

    checkResponseStatus(json_result)
    oauth2_code = json_result['data']['code']

    # 获取Cred凭证
    post_data = {
        'kind': 1,
        'code': oauth2_code
    }

    result = requests.post('https://zonai.skland.com/api/v1/user/auth/generate_cred_by_code', data=post_data)
    json_result = result.json()

    checkResponseCode(json_result)
    cred_cred = json_result['data']['cred']
    cred_userId = json_result['data']['userId']
    cred_token = json_result['data']['token']
    return cred_cred, cred_token


def header_get_sign(data: str, cred_token: str):
    '''
    获取 sign
    :param data: 要进行签名的数据，包括 url_path 和 url_body
    :param cred_token: 签名密钥
    :return: sign、header
    '''
    # 签名请求头一定要这个顺序，否则失败
    # timestamp是必填的,其它三个随便填,不要为none即可
    header_for_sign = {
        'platform': '1',
        'timestamp': str(int(time.time() - 2)),
        'dId': '0',
        'vName': '1.2.2'
    }
    str_to_sign = data + header_for_sign['timestamp'] + json.dumps(header_for_sign, separators=(',', ':'))
    hmac_hax = hmac.new(cred_token.encode('utf-8'), str_to_sign.encode('utf-8'), hashlib.sha256).hexdigest()
    md5 = hashlib.md5(hmac_hax.encode('utf-8')).hexdigest().encode('utf-8').decode('utf-8')

    print(f'{str_to_sign}\n=> sign={md5}')
    return md5, header_for_sign


def header_merge_with_sign(full_url: str, params: str, method: str, cred: str, cred_token: str):
    '''
    获取已签名的 headers
    :param full_url: 完整的 url
    :param params: post 数据，get 方式不需要提供
    :param method: 请求方法 'get' 或者 'post'
    :param cred: 
    :param cred_token: 
    :return: 完整的、已签名的 headers
    '''
    # 根据传入参数获取请求头
    url_part = parse.urlparse(full_url)

    md5 = ''
    header = ''
    if method.lower() == 'get':
        md5, header = header_get_sign(url_part.path + url_part.query, cred_token)
    elif method.lower() == 'post':
        md5, header = header_get_sign(url_part.path + params, cred_token)

    header['cred'] = cred
    header['sign'] = md5
    return header


def get_binding_list(cred: str, cred_token: str):
    '''
    查询账号绑定的角色 uid 和 channel
    :param cred: 
    :param cred_token: 
    :return: uid 和 channel
    '''
    # 绑定的角色url
    binding_url = "https://zonai.skland.com/api/v1/game/player/binding"

    result = requests.get(binding_url, headers=header_merge_with_sign(binding_url, None, 'get', cred, cred_token))
    json_result = result.json()

    checkResponseCode(json_result)
    return json_result['data']['list'][0]['bindingList'][0]['uid'], json_result['data']['list'][0]['bindingList'][0][
        'channelMasterId']


def check_in(binding_uid: str, binding_channel: str, cred: str, cred_token: str):
    '''
    进行签到
    :param binding_uid: 
    :param binding_channel: 
    :param cred: 
    :param cred_token: 
    :return: 
    '''
    # 签到url
    sign_url = "https://zonai.skland.com/api/v1/game/attendance"

    body = {
        'uid': binding_uid,
        'gameId': binding_channel
    }
    body_str = json.dumps(body)
    result = requests.post(sign_url, json=body,
                           headers=header_merge_with_sign(sign_url, body_str, 'post', cred, cred_token))

    json_result = result.json()

    print(json_result)

    return

# 森空岛自动签到
# 2023年10月6日
# 参考: https://github.com/Zerolouis/skyland_auto_sign_qinglong/blob/master/auto_sign.py#L123
if __name__ == '__main__':

    cred_cred = ''
    cred_token = ''

    if len(cred_cred) == 0 or len(cred_token) == 0:
        login_token = login('', '')
        cred_cred, cred_token = get_cred(login_token)
        print(f'cred_cred = \'{cred_cred}\'\ncred_token = \'{cred_token}\'')

    binding_uid, binding_channel = get_binding_list(cred_cred, cred_token)
    print(binding_uid, binding_channel)

    check_in(binding_uid, binding_channel, cred_cred, cred_token)
