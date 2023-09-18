from arduino.utils import ArduinoUtils
from arduino.commands import ArduinoCommands
from arduino.responses import ArduinoResponses
import hashlib
import sqlite3
from datetime import datetime

conn = sqlite3.connect('user.db')
curs = conn.cursor()

def info_msg():
    print('+---------------------------+')
    print('| 1) log mode               |')
    print('| 2) register user          |')
    print('| 3) delete user            |')
    print('| 4) list users             |')
    print('| 5) list log               |')
    print('| 6) * delete all users *   |')
    print('| 9) exit                   |')
    print('+---------------------------+')

def list_users():
    print('Listing users:')
    users = curs.execute('SELECT * FROM user')
    for i, user in enumerate(users.fetchall()):
        if user[2] == 1:
            print(f'User {i + 1}: name: {user[0]} - tag {user[1]}')
    input('press return to exit...')


def list_log():
    print('Listing log:')
    logs = curs.execute('SELECT * FROM log')
    for log in logs.fetchall():
        print(f'time: {log[0]}, user: {log[1]}')
    input('press return to exit...')
def serial_listener():
    print('Serial listener:')
    ArduinoUtils.clear_input_buffer()
    try:
        while True:
            response = ArduinoUtils.wait_for_response()
            res = ArduinoUtils.decode_response(response)
            print(f'arduino response: {res["message"]}')
            if not res['status']:
                print(f'error occurred')
                break
            if res['code'] == ArduinoResponses.USER_ACCESS:
                user_tag = ArduinoUtils.wait_for_response()
                print(f'User tag: {user_tag}')
                res = curs.execute('SELECT * FROM user WHERE tag = ? and used = 1', (user_tag, ))
                user = res.fetchone()
                if user is not None:
                    access_record = (datetime.now().strftime('%d-%m-%Y %H:%M:%S'), user[0], user[1])
                    curs.execute('INSERT INTO log VALUES (?, ?, ?)', access_record)
                    conn.commit()
                    print('log updated')
    except KeyboardInterrupt:
        return

def register_user():
    print('Register new user')
    try:
        name = input('Enter user full name: ')
        hash = hashlib.sha256(name.encode()).hexdigest()
        data = str(hash)[0:16]
        curs.execute('INSERT INTO user VALUES (?, ?, ?)', (name, data, 1))
        conn.commit()
        ArduinoUtils.clear_input_buffer()
        ArduinoUtils.send_command(ArduinoCommands.REGISTER_MODE)
        while True:
            response = ArduinoUtils.wait_for_response()
            res = ArduinoUtils.decode_response(response)
            if not res['code'] == ArduinoResponses.NO_FINGER:
                print(f'arduino response: {res["message"]}')
            if not res['status']:
                print(f'error occured')
                break
            if res['code'] == ArduinoResponses.CARD_PRESENT:
                ArduinoUtils.send_data(data)
            if res['code'] == ArduinoResponses.USER_REGISTERED:
                break
    except KeyboardInterrupt:
        return


def clear_databases():
    print('Deleting all users:')
    proceed = input('Delete all users: (Y/n): ')
    if proceed != 'Y':
        print('Aborting operation')

    curs.execute('DELETE FROM user')
    conn.commit()
    ArduinoUtils.clear_input_buffer()
    ArduinoUtils.send_command(ArduinoCommands.CLEAR_DB_MODE)
    response = ArduinoUtils.wait_for_response()
    res = ArduinoUtils.decode_response(response)
    print(f'arduino response: {res["message"]}')
    input('press return to exit...')

def delete_user():
    print('Delete user:')
    try:
        users = curs.execute('SELECT * FROM user')
        users = users.fetchall()
        n = len(users)
        options = []
        print('-------------------------')
        for i, user in enumerate(users):
            if user[2] == 1:
                print(f'User {i + 1}: name: {user[0]} - tag {user[1]}')
                options.append(i + 1)
        print('-------------------------')
        user_to_delete_index = -1
        try:
            user_to_delete_index = int(input(f'Choose user to be deleted: '))
        except ValueError:
            print('You selected incorrect user')
            return
        if user_to_delete_index not in options:
            print('You selected incorrect user')
            return
        user_to_delete = users[user_to_delete_index-1]
        proceed = input(f'Delete user: {user_to_delete[0]} - {user_to_delete[1]} (Y/n): ')
        if proceed != 'Y':
            print('Aborting operation')
            return

        ArduinoUtils.clear_input_buffer()

        ArduinoUtils.send_command(ArduinoCommands.DELETE_USER)
        while True:
            response = ArduinoUtils.wait_for_response()
            res = ArduinoUtils.decode_response(response)
            print(f'arduino response: {res["message"]}')
            if not res['status']:
                print(f'error occured')
                break
            if res['code'] == ArduinoResponses.WAITING_FOR_USER_ID:
                ArduinoUtils.send_command(user_to_delete_index)
                user_id = ArduinoUtils.wait_for_response()
                print(f'arduino response: deleting user {user_id}')
            if res['code'] == ArduinoResponses.USER_DELETED:
                break

        curs.execute('UPDATE user SET used = 0 WHERE tag = ?', (user_to_delete[1], ))
        conn.commit()

        print('User deleted successfully')
        input('press return to exit...')
    except KeyboardInterrupt:
        print('Aborting operation')
        return

if __name__ == '__main__':
    try:
        while True:
            info_msg()
            choice = int(input('Your choice: '))
            if choice == 1:
                serial_listener()
            if choice == 2:
                register_user()
            if choice == 3:
                delete_user()
            if choice == 4:
                list_users()
            if choice == 5:
                list_log()
            if choice == 6:
                clear_databases()
            if choice == 9:
                ArduinoUtils.close_connection()
                break
    except KeyboardInterrupt:
        ArduinoUtils.close_connection()