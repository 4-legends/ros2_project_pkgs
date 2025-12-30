from setuptools import find_packages, setup

package_name = 'number_pub_and_counter_py'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='legends',
    maintainer_email='legends@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            "number_publisher = number_pub_and_counter_py.number_publisher:main",
            "number_counter = number_pub_and_counter_py.number_counter:main"
        ],
    },
)
