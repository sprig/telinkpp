/** \file telink_mesh.h
 *  Class handling connection with a Bluetooth LE device with Telink mesh protocol.
 *  Author: Vincent Paeder
 *  License: GPL v3
 */
#ifndef __TELINK_MESH_H__
#define __TELINK_MESH_H__

#include <string>
#include <vector>
#include <exception>
#include <tinyb.hpp>

namespace telink {
  
  #define schar(x) static_cast<char>(x)
  
  /** \brief UUID for Bluetooth GATT information service */
  static std::string uuid_info_service = "00010203-0405-0607-0809-0a0b0c0d1910";
  /** \brief UUID for Bluetooth GATT notification characteristic */
  static std::string uuid_notification_char = "00010203-0405-0607-0809-0a0b0c0d1911";
  /** \brief UUID for Bluetooth GATT command characteristic */
  static std::string uuid_command_char = "00010203-0405-0607-0809-0a0b0c0d1912";
  /** \brief UUID for Bluetooth GATT pairing characteristic */
  static std::string uuid_pair_char = "00010203-0405-0607-0809-0a0b0c0d1914";
  
  // Command codes
  #define COMMAND_OTA_UPDATE            0xC6
  #define COMMAND_QUERY_OTA_STATE       0xC7
  #define COMMAND_OTA_STATUS_REPORT     0xC8
  #define COMMAND_GROUP_ID_QUERY        0xDD
  #define COMMAND_GROUP_ID_REPORT       0xD4
  #define COMMAND_GROUP_EDIT            0xD7
  #define COMMAND_ONLINE_STATUS_REPORT  0xDC
  #define COMMAND_ADDRESS_EDIT          0xE0
  #define COMMAND_ADDRESS_REPORT        0xE1
  #define COMMAND_RESET                 0xE3
  #define COMMAND_TIME_QUERY            0xE8
  #define COMMAND_TIME_REPORT           0xE9
  #define COMMAND_TIME_SET              0xE4
  #define COMMAND_DEVICE_INFO_QUERY     0xEA
  #define COMMAND_DEVICE_INFO_REPORT    0xEB
  
  class TelinkMeshException : public std::exception {
  private:
    std::string message;
  public:
    TelinkMeshException(const std::string message) throw() : message(message) {}
    virtual const char* what() const throw() override {
      return ("TelinkMeshException occurred: " + this->message + "\n").c_str();
    }
  };
  
  /** \class TelinkMesh
   *  \brief Class handling connection with a Bluetooth LE device with Telink mesh protocol.
   */
  class TelinkMesh {
  private:
    /** \property std::string address
     *  \brief MAC address of Bluetooth device.
     */
    std::string address;
    
    /** \property std::string reverse_address
     *  \brief MAC address formatted for little-endianness.
     */
    std::string reverse_address;
    
    /** \property std::string name
     *  \brief Device name.
     */
    std::string name;
    
    /** \property std::string password
     *  \brief Device password. Used to generate shared key.
     */
    std::string password;
    
    /** \property std::string shared_key
     *  \brief Shared key used to encrypt communication with device.
     */
    std::string shared_key;
    
    /** \property int vendor
     *  \brief Bluetooth vendor code.
     */
    int vendor = 0x211;
    
    /** \property int mesh_id
     *  \brief Device ID.
     */
    int mesh_id = 0;
    
    /** \property int packet_count
     *  \brief Packet counter used to tag transmitted packets.
     */
    int packet_count = 1;
  
    /** \property std::unique_ptr<BluetoothDevice> ble_mesh
     *  \brief TinyB Bluetooth device object.
     */
    std::unique_ptr<BluetoothDevice> ble_mesh;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> notification_char
     *  \brief TinyB object for notification Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> notification_char;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> command_char
     *  \brief TinyB object for command Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> command_char;
    
    /** \property std::unique_ptr<BluetoothGattCharacteristic> pair_char
     *  \brief TinyB object for pairing Bluetooth GATT characteristic.
     */
    std::unique_ptr<BluetoothGattCharacteristic> pair_char;
  
    /** \fn std::string combine_name_and_password()
     *  \brief Combines the device name and password for use with shared key generation.
     *  \returns a string containing combined device name and password.
     */
    std::string combine_name_and_password() const;
    
    /** \fn void generate_shared_key(const std::string & data1, const std::string & data2)
     *  \brief Generates a shared key from given data. Result is stored in member variable shared_key.
     *  \param data1 : 8-byte string.
     *  \param data2 : another 8-byte string.
     */
    void generate_shared_key(const std::string & data1, const std::string & data2);
    
    /** \fn std::string key_encrypt(std::string & key) const
     *  \brief Encrypts device name/password combination with given key.
     *  \param key : 16-byte string used as encryption key.
     *  \returns a 16-byte string containing the encrypted name/password combination.
     */
    std::string key_encrypt(std::string & key) const;
    
    /** \fn std::string encrypt_packet(std::string & packet) const
     *  \brief Encrypts given packet with stored shared key.
     *  \param packet : 20-byte packet to encrypt.
     *  \returns the encrypted 20-byte packet.
     */
    std::string encrypt_packet(std::string & packet) const;
    
    /** \fn std::string decrypt_packet(std::string & packet) const
     *  \brief Decrypts given packet with stored shared key.
     *  \param packet : 20-byte packet to decrypt.
     *  \returns the decrypted 20-byte packet.
     */
    std::string decrypt_packet(std::string & packet) const;
    
    /** \fn std::string build_packet(int command, const std::string & data)
     *  \brief Builds a command packet to be sent to the device.
     *  \param command : command code.
     *  \param data : command parameters (up to 10 byte).
     *  \returns the encrypted generated packet.
     */
    std::string build_packet(int command, const std::string & data);
  
    /** \fn void notification_callback(BluetoothGattCharacteristic & c, std::vector<unsigned char> & data, void * userdata)
     *  \brief Callback for notification Bluetooth GATT characteristic.
     *  \param c : GATT characteristic that received data.
     *  \param data : received data.
     */
    void notification_callback(BluetoothGattCharacteristic & c, std::vector<unsigned char> & data);
  
  protected:
    /** \fn virtual void parse_command(const std::string & packet)
     *  \brief Parses a command packet.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_command(const std::string & packet);
    
  public:
    /** \fn TelinkMesh(const std::string address)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     */
    TelinkMesh(const std::string address);
    /** \fn TelinkMesh(const std::string address, const std::string name, const std::string password)
     *  \brief Object instantiation.
     *  \param address : device MAC address.
     *  \param name : device name.
     *  \param password : device password.
     */
    TelinkMesh(const std::string address, const std::string name, const std::string password);
    
    ~TelinkMesh();
    
    /** \fn void set_address(const std::string address)
     *  \brief Sets the MAC address to connect to.
     *  \param address : MAC address in the form AA:BB:CC:DD:EE:FF.
     */
    void set_address(const std::string address);
    
    /** \fn void set_name(const std::string name)
     *  \brief Sets the device name to be used for connecting.
     *  \param name : device name.
     */
    void set_name(const std::string name);
    
    /** \fn void set_password(const std::string password)
     *  \brief Sets the password to be used for connecting.
     *  \param password : password.
     */
    void set_password(const std::string password);
    
    /** \fn void set_vendor(int vendor)
     *  \brief Sets the Bluetooth vendor code (0x0211 for Telink).
     *  \param vendor : vendor code.
     */
    void set_vendor(int vendor);
    
    /** \fn void send_packet(int command, const std::string & data)
     *  \brief Sends a command packet to the device.
     *  \param command : command code.
     *  \param data : command parameters (up to 10 byte).
     */
    void send_packet(int command, const std::string & data);
  
    /** \fn bool connect()
     *  \brief Connects to Bluetooth device.
     *  \returns true if connection succeeded, false otherwise.
     */
    bool connect();
    
    /** \fn void disconnect()
     *  \brief Disconnects from Bluetooth device.
     */
    void disconnect();
    
    /** \fn bool is_connected()
     *  \brief Probes whether the connection with the device is established.
     *  \returns true if connected, false otherwise.
     */
    bool is_connected();
    
    /** \fn void query_mesh_id()
     *  \brief Queries mesh ID from device.
     */
    void query_mesh_id();
   
    /** \fn void query_groups()
     *  \brief Queries mesh group IDs from device.
     */
    void query_groups();
   
    /** \fn void set_time()
     *  \brief Sets device date and time.
     */
    void set_time();
   
    /** \fn void query_time()
     *  \brief Queries device date and time.
     */
    void query_time();
    
    /** \fn void query_device_info()
     *  \brief Queries device information.
     */
    void query_device_info();
    
    /** \fn void query_device_version()
     *  \brief Queries device firmware version.
     */
    void query_device_version();
    
    /** \fn void set_mesh_id(int mesh_id)
     *  \brief Sets device mesh ID.
     *  \param mesh_id : mesh ID to set, from 1 to 254 for single device ID, and from 0x8000 to 0x80ff for group ID
     */
    void set_mesh_id(int mesh_id);
    
    /** \fn void add_group(unsigned char group_id)
     *  \brief Adds device to given group.
     *  \param group_id : ID of the group to add device to.
     */
    void add_group(unsigned char group_id);
    
    /** \fn void delete_group(unsigned char group_id)
     *  \brief Removes device from given group.
     *  \param group_id : ID of the group to remove device from.
     */
    void delete_group(unsigned char group_id);
    
    bool check_packet_validity(const std::string & packet);
    
    /** \fn virtual void parse_time_report(const std::string & packet)
     *  \brief Parses a command packet from a time report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_time_report(const std::string & packet);
    
    /** \fn virtual void parse_address_report(const std::string & packet)
     *  \brief Parses a command packet from an address report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_address_report(const std::string & packet);
    
    /** \fn virtual void parse_device_info_report(const std::string & packet)
     *  \brief Parses a command packet from a device info report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_device_info_report(const std::string & packet);
    
    /** \fn virtual void parse_group_id_report(const std::string & packet)
     *  \brief Parses a command packet from a group ID report.
     *  \param packet : decrypted packet to be parsed.
     */
    virtual void parse_group_id_report(const std::string & packet);
  };
  
}

#endif // __TELINK_MESH_H__
