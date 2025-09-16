/**
 * @file MS5837.h
 *
 */

#ifndef MS5837_H
#define MS5837_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

#define MS5837_ADDR               0x76  
#define MS5837_RESET              0x1E
#define MS5837_ADC_READ           0x00
#define MS5837_PROM_READ          0xA0

// Pressure Conversion
#define MS5837_CONVERT_D1_256     0x40
#define MS5837_CONVERT_D1_512     0x42
#define MS5837_CONVERT_D1_1024    0x44
#define MS5837_CONVERT_D1_2048    0x46
#define MS5837_CONVERT_D1_4096    0x48
#define MS5837_CONVERT_D1_8192    0x4A

// Temperature Conversion
#define MS5837_CONVERT_D2_256     0x50
#define MS5837_CONVERT_D2_512     0x52
#define MS5837_CONVERT_D2_1024    0x54
#define MS5837_CONVERT_D2_2048    0x56
#define MS5837_CONVERT_D2_4096    0x58
#define MS5837_CONVERT_D2_8192    0x5A

#define MS5837_30BA  0
#define MS5837_02BA  1
#define MS5837_UNRECOGNISED  255

#define MS5837_VERSION_02BA01  0x00 // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
#define MS5837_VERSION_02BA21  0x15 // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
#define MS5837_VERSION_30BA26  0x1A // Sensor version: From MS5837_30BA datasheet Version PROM Word 0

#define MS5837_SEAWATER_DENSITY   1029.0
#define MS5837_FRESHWATER_DENSITY 997.0

#define MS5837_DEFAULT_SEA_LEVEL_AIR_PRESSURE 101300

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

enum MS5837_OVERSAMPLING : unsigned int
{
	OSR_256      = 0,
	OSR_512      = 1,
	OSR_1024     = 2,
	OSR_2048     = 3,
	OSR_4096     = 4,
	OSR_8192     = 5
};
class MS5837 {
public:

	static const float Pa;
	static const float bar;
	static const float mbar;

    //Constructor
	MS5837();

    /*
    * @brief Initialize MS5837 and set basic configurations
    * @details
    *  Init configuration:
    *  - Seawater density
    *
    * @note
    *  - A soft reset is performed first, which takes ~10ms.
    */
	bool initialize();

    /*
     * @brief Set model of MS5837 sensor. 
     * @details 
     * Valid options are MS5837_30BA
	 * and MS5837_02BA.
	 */
	void setModel(uint8_t model);

    /*
     * @brief Get model logged for MS5837 sensor. 
     * @details 
     * Valid options are MS5837_30BA
	 * and MS5837_02BA.
	 */
    uint8_t getModel();

    /*
     * @brief Set level of oversampling
	 */
    void setOverSampling(MS5837_OVERSAMPLING oversampling);

    /*
     * @brief Set sea level air pressure
	 */
	void setSeaLevelAirPressure(float pressure);

    /*
     * @brief Get current sea level air pressure record
	 */
    float getSeaLevelAirPressure();

    /*
    * @brief Take a reading
    * @details
    *  Reads the sensor, using oversampling as configured
    * 
    * @param current time in microseconds
    *
    * @note
    * To avoid delay this gets called several times until the process completes
    * which could be 40ms later at highest oversampling
    *  
    */    
    int8_t read_nonblocking(int64_t time_us);

    /*
    * @brief Take a reading
    * @details
    *  Reads the sensor, using oversampling as configured
    *
    * @note
    * Can take up to 40ms to return for highest oversampling,
    *  
    */    
    bool read();

    /** Pressure returned in mbar or mbar*conversion rate.
	 */
	float pressure(float conversion = 1.0f);

	/** Temperature returned in deg C.
	 */
	float temperature();

	/** Depth returned in meters (valid for operation in incompressible
	 *  liquids only. Uses density that is set for fresh or seawater.
	 */
	float depth();

private:
	uint8_t _model;
    uint8_t _active_request = 0; // 0:No request, 1: Pressure request, 2: Temperature request, 3: Complete
    int64_t _readStartTime; // The time for the start of each read cycle
	uint16_t C[8]; // Stores calibration data
	uint32_t D1, D2; // Stores the raw readings
    int32_t TEMP;
	int32_t P;

    uint32_t seaLevelAirPressure;
    float fluidDensity;

    // Default value of oversampling is set to 8192
    
    uint8_t _oversampling = MS5837_OVERSAMPLING::OSR_8192;

	// 2.5 * pow(2, 8+_oversampling)
    // Lookup is in microseconds
    int delay_lookup[6] {640, 1280, 2560, 5120, 10240, 20480};
    int delay_lookup_ms[6] {1, 2, 3, 6, 11, 21};

    /* 
     * Performs calculations per the sensor data sheet for conversion and
	 * second order compensation.
	 */
	void calculate();

    uint8_t crc4(uint16_t n_prom[]);
};

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // MS5837_H 